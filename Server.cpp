#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <processthreadsapi.h>
#include <mmsystem.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "winmm.lib")

SOCKET sock;
HWND hwMain;
bool bSocket = false;

// ����M������W�f�[�^
struct POS
{
	int x;
	int y;
};
POS pos1P, pos2P, old_pos2P;
RECT rect;


// �v���g�^�C�v�錾
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

	// winsock������
	WSAStartup(MAKEWORD(1, 1), &wdData);

	hwMain = CreateWindow("CWindow", "Server",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, NULL, NULL, hInstance, NULL);

	// �E�C���h�E�\��
	ShowWindow(hwMain, iCmdShow);

	// �E�B���h�E�̈�X�V(WM_PAINT���b�Z�[�W�𔭍s)
	UpdateWindow(hwMain);

	// ���b�Z�[�W���[�v
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// winsock�I��
	WSACleanup();

	return (int)msg.wParam;
}

// �E�C���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	static HDC hdc, mdc, mdc2P;
	static PAINTSTRUCT ps;
	static HBITMAP hBitmap;
	static HBITMAP hBitmap2P;
	static DWORD dwID;
	static char str[256];

	// WINDOWS������ŗ��郁�b�Z�[�W�ɑΉ����鏈���̋L�q
	switch (iMsg) {
	case WM_CREATE:
		// ���\�[�X����r�b�g�}�b�v��ǂݍ��ށi1P�j
		hBitmap = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance, "MAJO");

		// �f�B�X�v���C�ƌ݊����̂���_���f�o�C�X�i�f�o�C�X�R���e�L�X�g�j���擾
		mdc = CreateCompatibleDC(NULL);

		// �_���f�o�C�X�ɓǂݍ��񂾃r�b�g�}�b�v��W�J
		SelectObject(mdc, hBitmap);

		// ���\�[�X����r�b�g�}�b�v��ǂݍ��ށi2P�j
		hBitmap2P = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance,
			"MAJO2P");

		// 1P�Ɠ���
		mdc2P = CreateCompatibleDC(NULL);
		// 1P�Ɠ���
		SelectObject(mdc2P, hBitmap2P);

		pos1P.x = pos1P.y = 0;
		pos2P.x = pos2P.y = 100;

		// �f�[�^�𑗎�M�������X���b�h�iWinMain�̗���Ɋ֌W�Ȃ����삷�鏈���̗���j�Ƃ��Đ����B
		// �f�[�^����M���X���b�h�ɂ��Ȃ��Ɖ����f�[�^����M����܂�RECV�֐��Ŏ~�܂��Ă��܂��B
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

		// �E�B���h�E���X�V�̈�ɒǉ�
		// hWnd	 �F	�E�C���h�E�̃n���h��
		// lprec �FRECT�̃|�C���^�DNULL�Ȃ�S��
		// bErase�FTRUE�Ȃ�X�V�̈��w�i�F�ŏ������C FALSE�Ȃ猻�݂̏�Ԃ���㏑���`��
		// �Ԃ�l�F���������TRUE�C����ȊO��FALSE
		InvalidateRect(hwnd, NULL, TRUE);

		// WM_PAINT���b�Z�[�W���E�B���h�E�ɑ��M�����
		UpdateWindow(hwnd);
		break;
	case WM_PAINT:
		// �X�V�̈�ɕ`�悷��ׂɕK�v�ȕ`��c�[���i�f�o�C�X�R���e�L�X�g�j���擾
		hdc = BeginPaint(hwnd, &ps);

		// �]�����f�o�C�X�R���e�L�X�g����]����f�o�C�X�R���e�L�X�g��
		// �����`�J���[�f�[�^�̃r�b�g�u���b�N��]��
		BitBlt(hdc, pos1P.x, pos1P.y, 32, 32, mdc, 0, 0, SRCCOPY);
		BitBlt(hdc, pos2P.x, pos2P.y, 32, 32, mdc2P, 0, 0, SRCCOPY);

		wsprintf(str, "X:%d Y:%d", pos1P.x, pos1P.y);
		SetWindowText(hwMain, str);

		// �X�V�̈����ɂ���
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY: /* �E�C���h�E�j���� */
		DeleteObject(hBitmap);
		DeleteDC(mdc);
		DeleteObject(hBitmap2P);
		DeleteDC(mdc2P);

		// �\�P�b�g����Ă�����J��
		if (bSocket) {
			shutdown(sock, 2);
			closesocket(sock);
		}

		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/* �ʐM�X���b�h�֐� */
DWORD WINAPI threadfunc(void* px) {

	int fromlen, recv_cnt, send_cnt;
	struct sockaddr_in recv_addr, addr;

	// �\�P�b�g
	sock = socket(PF_INET, SOCK_DGRAM, 0);

	// �\�P�b�g�Ƀ|�[�g�ԍ��ݒ�
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		SetWindowText(hwMain, "�\�P�b�g���s");
		return 1;
	}

	// �\�P�b�g�쐬�t���O�Z�b�g
	bSocket = true;

	while (1)
	{
		fromlen = sizeof(recv_addr);
		recv_cnt = send_cnt = 0;

		old_pos2P = pos2P;

		// �f�[�^��M
		recv_cnt = recvfrom(sock, (char*)&pos2P, sizeof(POS), 0, (struct sockaddr*)&recv_addr, &fromlen);

		while (send_cnt == 0)
		{
			// ���b�Z�[�W���M
			send_cnt = sendto(sock, (const char*)&pos1P, sizeof(POS), 0, (struct sockaddr*)&recv_addr, sizeof(recv_addr));
		}

		// ��M�����N���C�A���g�����삷��L�����̍��W���X�V����Ă�����A�X�V�̈�������
		// InvalidateRect�֐���WM_PAINT���b�Z�[�W�𔭍s�A�L�������ĕ`�悷��
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