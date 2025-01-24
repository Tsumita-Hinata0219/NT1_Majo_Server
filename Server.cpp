#include <Windows.h>
#include <WinUser.h>
#include "resource.h"

// 構造体
struct VEC2
{
	int x, y;
};

// グローバル変数
const char* title = "Server";
HWND hwnd;
WNDCLASS wndclass;
RECT wrc;

// プロトタイプ宣言
void MyRegisterClass(HINSTANCE);
void SettingWindowSize();
void CreateAndShowWindow(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/// <summary>
/// エントリーポイント : WinMain
/// </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	/* ウィンドウクラスを登録する */
	MyRegisterClass(hInstance);

	/* ウィンドウサイズを決める */
	SettingWindowSize();

	/* ウィンドウを生成して表示 */
	CreateAndShowWindow(hInstance, nCmdShow);

	/* ループ */
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO));
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}


/// <summary>
/// ウィンドウの仕様&登録
/// </summary>
void MyRegisterClass(HINSTANCE hInstance)
{
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Server";

	// ウィンドウクラスを登録する
	RegisterClass(&wndclass);
}


/// <summary>
/// ウィンドウサイズを決める
/// </summary>
void SettingWindowSize()
{
	wrc = { 0, 0, 640, 640 };
	// クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
}


/// <summary>
/// ウィンドウを生成して表示
/// </summary>
void CreateAndShowWindow(HINSTANCE hInstance, int nCmdShow)
{
	hwnd = CreateWindow(
		wndclass.lpszClassName, title,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		wrc.right - wrc.left, wrc.bottom - wrc.top,
		NULL, NULL, hInstance, NULL);

	// ウインドウ表示
	ShowWindow(hwnd, nCmdShow);

	// ウィンドウ領域更新(WM_PAINTメッセージを発行)
	UpdateWindow(hwnd);
}


/// <summary>
/// ウィンドウプロシージャ
/// </summary>
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitMap1P, hBitMap2P; // LoadBitMapの帰り値
	static HDC mdc1P, mdc2P; // ビトッマプ毎のメモリデバイスコンテキスト

	switch (iMsg) {

		/* ───────── ウィンドウ生成 */
	case WM_CREATE:
		// リソースからビットマップを読み込む（1P）
		hBitMap1P = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance, "majo_blue.bmp");
		// 論理メモリ作成
		mdc1P = CreateCompatibleDC(NULL);
		// 論理メモリに世も込んだビットマップを展開
		SelectObject(mdc1P, hBitMap1P);

		// リソースからビットマップを読み込む（2P）
		hBitMap2P = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance, "majo_blue.bmp");
		// 論理メモリ作成
		mdc2P = CreateCompatibleDC(NULL);
		// 論理メモリに世も込んだビットマップを展開
		SelectObject(mdc2P, hBitMap2P);

	
		break;
		
		/* ───────── ウィンドウが破棄 */
	case WM_DESTROY:
		// 作ったものはちゃんと破棄する
		DeleteObject(hBitMap1P);
		DeleteDC(mdc1P);
		DeleteObject(hBitMap2P);
		DeleteDC(mdc2P);

		// OSに対して、アプリ終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準メッセージ処理を行う
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}