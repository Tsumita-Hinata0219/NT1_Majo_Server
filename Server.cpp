#include <Windows.h>
#include <WinUser.h>
#include "resource.h"

// �\����
struct VEC2
{
	int x, y;
};

// �O���[�o���ϐ�
const char* title = "Server";
HWND hwnd;
WNDCLASS wndclass;
RECT wrc;

// �v���g�^�C�v�錾
void MyRegisterClass(HINSTANCE);
void SettingWindowSize();
void CreateAndShowWindow(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/// <summary>
/// �G���g���[�|�C���g : WinMain
/// </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	/* �E�B���h�E�N���X��o�^���� */
	MyRegisterClass(hInstance);

	/* �E�B���h�E�T�C�Y�����߂� */
	SettingWindowSize();

	/* �E�B���h�E�𐶐����ĕ\�� */
	CreateAndShowWindow(hInstance, nCmdShow);

	/* ���[�v */
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
/// �E�B���h�E�̎d�l&�o�^
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

	// �E�B���h�E�N���X��o�^����
	RegisterClass(&wndclass);
}


/// <summary>
/// �E�B���h�E�T�C�Y�����߂�
/// </summary>
void SettingWindowSize()
{
	wrc = { 0, 0, 640, 640 };
	// �N���C�A���g�̈�����ƂɎ��ۂ̃T�C�Y��wrc��ύX���Ă��炤
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
}


/// <summary>
/// �E�B���h�E�𐶐����ĕ\��
/// </summary>
void CreateAndShowWindow(HINSTANCE hInstance, int nCmdShow)
{
	hwnd = CreateWindow(
		wndclass.lpszClassName, title,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		wrc.right - wrc.left, wrc.bottom - wrc.top,
		NULL, NULL, hInstance, NULL);

	// �E�C���h�E�\��
	ShowWindow(hwnd, nCmdShow);

	// �E�B���h�E�̈�X�V(WM_PAINT���b�Z�[�W�𔭍s)
	UpdateWindow(hwnd);
}


/// <summary>
/// �E�B���h�E�v���V�[�W��
/// </summary>
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitMap1P, hBitMap2P; // LoadBitMap�̋A��l
	static HDC mdc1P, mdc2P; // �r�g�b�}�v���̃������f�o�C�X�R���e�L�X�g

	switch (iMsg) {

		/* ������������������ �E�B���h�E���� */
	case WM_CREATE:
		// ���\�[�X����r�b�g�}�b�v��ǂݍ��ށi1P�j
		hBitMap1P = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance, "majo_blue.bmp");
		// �_���������쐬
		mdc1P = CreateCompatibleDC(NULL);
		// �_���������ɐ������񂾃r�b�g�}�b�v��W�J
		SelectObject(mdc1P, hBitMap1P);

		// ���\�[�X����r�b�g�}�b�v��ǂݍ��ށi2P�j
		hBitMap2P = LoadBitmap(
			((LPCREATESTRUCT)lParam)->hInstance, "majo_blue.bmp");
		// �_���������쐬
		mdc2P = CreateCompatibleDC(NULL);
		// �_���������ɐ������񂾃r�b�g�}�b�v��W�J
		SelectObject(mdc2P, hBitMap2P);

	
		break;
		
		/* ������������������ �E�B���h�E���j�� */
	case WM_DESTROY:
		// ��������̂͂����Ɣj������
		DeleteObject(hBitMap1P);
		DeleteDC(mdc1P);
		DeleteObject(hBitMap2P);
		DeleteDC(mdc2P);

		// OS�ɑ΂��āA�A�v���I����`����
		PostQuitMessage(0);
		return 0;
	}

	// �W�����b�Z�[�W�������s��
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}