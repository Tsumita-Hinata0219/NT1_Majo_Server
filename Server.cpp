#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <processthreadsapi.h>
#include <mmsystem.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "winmm.lib")

SOCKET sock;
HWND hwMain;
bool bSocket = false;

// 送受信する座標データ
struct POS
{
	int x;
	int y;
};
POS pos1P, pos2P, old_pos2P;
RECT rect;


// プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI threadfunc(void*);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow) {

	MSG  msg;
	WNDCLASS wndclass;
	WSADATA wdData;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "CWindow";

	RegisterClass(&wndclass);

	// winsock初期化
	WSAStartup(MAKEWORD(1, 1), &wdData);

	hwMain = CreateWindow("CWindow", "Server",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, NULL, NULL, hInstance, NULL);

	// ウインドウ表示
	ShowWindow(hwMain, iCmdShow);

	// ウィンドウ領域更新(WM_PAINTメッセージを発行)
	UpdateWindow(hwMain);

	// メッセージループ
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// winsock終了
	WSACleanup();

	return (int)msg.wParam;
}

// ウインドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	static HDC hdc, mdc, mdc2P;
	static PAINTSTRUCT ps;
	static HBITMAP hBitmap;
	static HBITMAP hBitmap2P;
	static DWORD dwID;
	static char str[256];

	// WINDOWSから飛んで来るメッセージに対応する処理の記述
	switch (iMsg) {
	case WM_CREATE:
		// リソースからビットマップを読み込む（1P）
		hBitmap = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance, "MAJO");

		// ディスプレイと互換性のある論理デバイス（デバイスコンテキスト）を取得
		mdc = CreateCompatibleDC(NULL);

		// 論理デバイスに読み込んだビットマップを展開
		SelectObject(mdc, hBitmap);

		// リソースからビットマップを読み込む（2P）
		hBitmap2P = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance,
			"MAJO2P");

		// 1Pと同じ
		mdc2P = CreateCompatibleDC(NULL);
		// 1Pと同じ
		SelectObject(mdc2P, hBitmap2P);

		pos1P.x = pos1P.y = 0;
		pos2P.x = pos2P.y = 100;

		// データを送受信処理をスレッド（WinMainの流れに関係なく動作する処理の流れ）として生成。
		// データ送受信をスレッドにしないと何かデータを受信するまでRECV関数で止まってしまう。
		CreateThread(NULL, 0, threadfunc, (LPVOID)&pos1P, 0, &dwID);

		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			break;
		case VK_RIGHT:
			pos1P.x += 5;
			break;
		case VK_LEFT:
			pos1P.x -= 5;
			break;
		case VK_DOWN:
			pos1P.y += 5;
			break;
		case VK_UP:
			pos1P.y -= 5;
			break;
		}

		// ウィンドウを更新領域に追加
		// hWnd	 ：	ウインドウのハンドル
		// lprec ：RECTのポインタ．NULLなら全体
		// bErase：TRUEなら更新領域を背景色で初期化， FALSEなら現在の状態から上書き描画
		// 返り値：成功すればTRUE，それ以外はFALSE
		InvalidateRect(hwnd, NULL, TRUE);

		// WM_PAINTメッセージがウィンドウに送信される
		UpdateWindow(hwnd);
		break;
	case WM_PAINT:
		// 更新領域に描画する為に必要な描画ツール（デバイスコンテキスト）を取得
		hdc = BeginPaint(hwnd, &ps);

		// 転送元デバイスコンテキストから転送先デバイスコンテキストへ
		// 長方形カラーデータのビットブロックを転送
		BitBlt(hdc, pos1P.x, pos1P.y, 32, 32, mdc, 0, 0, SRCCOPY);
		BitBlt(hdc, pos2P.x, pos2P.y, 32, 32, mdc2P, 0, 0, SRCCOPY);

		wsprintf(str, "X:%d Y:%d", pos1P.x, pos1P.y);
		SetWindowText(hwMain, str);

		// 更新領域を空にする
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY: /* ウインドウ破棄時 */
		DeleteObject(hBitmap);
		DeleteDC(mdc);
		DeleteObject(hBitmap2P);
		DeleteDC(mdc2P);

		// ソケット作られていたら開放
		if (bSocket) {
			shutdown(sock, 2);
			closesocket(sock);
		}

		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/* 通信スレッド関数 */
DWORD WINAPI threadfunc(void* px) {

	int fromlen, recv_cnt, send_cnt;
	struct sockaddr_in recv_addr, addr;

	// ソケット
	sock = socket(PF_INET, SOCK_DGRAM, 0);

	// ソケットにポート番号設定
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		SetWindowText(hwMain, "ソケット失敗");
		return 1;
	}

	// ソケット作成フラグセット
	bSocket = true;

	while (1)
	{
		fromlen = sizeof(recv_addr);
		recv_cnt = send_cnt = 0;

		old_pos2P = pos2P;

		// データ受信
		recv_cnt = recvfrom(sock, (char*)&pos2P, sizeof(POS), 0, (struct sockaddr*)&recv_addr, &fromlen);

		while (send_cnt == 0)
		{
			// メッセージ送信
			send_cnt = sendto(sock, (const char*)&pos1P, sizeof(POS), 0, (struct sockaddr*)&recv_addr, sizeof(recv_addr));
		}

		// 受信したクライアントが操作するキャラの座標が更新されていたら、更新領域を作って
		// InvalidateRect関数でWM_PAINTメッセージを発行、キャラを再描画する
		if (old_pos2P.x != pos2P.x || old_pos2P.y != pos2P.y)
		{
			rect.left = old_pos2P.x - 10;
			rect.top = old_pos2P.y - 10;
			rect.right = old_pos2P.x + 42;
			rect.bottom = old_pos2P.y + 42;
			InvalidateRect(hwMain, &rect, TRUE);
		}
	}

	return 0;
}