/*------------------------------------------------------------------------*/
//								WinApp.cpp
/*------------------------------------------------------------------------*/

//#pragma comment ( linker, "/entry:WinMainCRTStartup /subsystem:console" )

/*------------------------------------------------------------------------*/
//								INCLUDES
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "WinApp.h"
#include "main.h"
#include "KeyboardMgr.h"
#include "MouseMgr.h"
#include "Draw.h"
#include "Menu.h"


/*------------------------------------------------------------------------*/
//								GLOBALS
/*------------------------------------------------------------------------*/



/*------------------------------------------------------------------------*/

App::App()
{

}

App::~App()
{
	Term();
}

BOOL App::Initialize()
{
	InitWindow();									// Window ���� �ʱ�ȭ

	pDxInput = new CDxInput;						// Input ��ü
	if (!pDxInput)
		return FALSE;

	pDXDriver = new CDxDriver;						// Driver ��ü
	if (!pDXDriver)
		return FALSE;

	pDxInput->Initialize(g_hInstance, g_hWnd);
	if (!pDXDriver->Initialize(g_hWnd))
	{
		MessageBox(NULL, TEXT("DX Initialize Error"), TEXT("DX Initialize Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

VOID App::Term()
{
	if (pDXDriver)
	{
		delete pDXDriver;							// Driver ��ü ��ȯ
		pDXDriver = nullptr;
	}

	if (pDxInput)
	{
		delete pDxInput;							// Input ��ü ��ȯ
		pDxInput = nullptr;
	}

	ShutDownWindow();								// Window ����
}

VOID App::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));									// msg init

	while (msg.message != WM_QUIT)
	{
		ShowWindow(g_hWnd, SW_SHOWDEFAULT);								// SW_SHOWMAXIMIZED  SW_SHOWMINIMIZED
		UpdateWindow(g_hWnd);
		//SetFocus(g_hWnd);

		QueryPerformanceFrequency(&m_frequency);							// CPU ���ļ��� ���� 1�ʴ� ����Ǵ� ƽ �� (10000000)
	//	m_fps = (double)m_frequency.QuadPart / FPS;						// �ʴ� frame ���� ����

		QueryPerformanceCounter(&m_lastTime);								// ���� ����


		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
			if (duringTime > (double)FPS)
				Sleep(1);

			QueryPerformanceCounter(&m_currentTime);											// �� ���� (����)
			clocks = static_cast<double>(m_currentTime.QuadPart - m_lastTime.QuadPart);			// Ŭ�� ��

			duringTime = clocks / (double)m_frequency.QuadPart * 1000.0;
			//		duringTime = clocks / (m_fps) * 1000.0;											// �� ���� ����ð� * FPS
			if (pDXDriver != nullptr)
			{
				pDXDriver->pDraw->SetDuringTime(static_cast<float>(duringTime));
			}
			m_lastTime = m_currentTime;
		}
	}
}

VOID App::InitWindow()
{
	WNDCLASSEX wndclass;
	memset(&wndclass, 0, sizeof(wndclass));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)::GetStockObject(DKGRAY_BRUSH);
	wndclass.hCursor = LoadCursor(::GetModuleHandle(NULL), IDC_ARROW);
	wndclass.hIcon = wndclass.hIconSm = LoadIcon(::GetModuleHandle(NULL), IDI_APPLICATION);
	g_hInstance = ::GetModuleHandle(NULL);
	wndclass.hInstance = g_hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = TEXT("Task");
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	::RegisterClassEx(&wndclass);

	int monitorX = (static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2)) - nWidth / 2;
	int monitorY = (static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 2)) - nHeight / 2;


	RECT rect;
	SetRect(&rect, 0, 0, nWidth, nHeight);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = ::CreateWindowEx(0, TEXT("Task"), TEXT("Task"), WS_OVERLAPPEDWINDOW, monitorX, monitorY, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, ::GetModuleHandle(NULL), NULL);


	//	DWORD d = GetLastError();



	return;
}

VOID App::ShutDownWindow()
{
	g_hWnd = NULL;

	UnregisterClass(L"Task", g_hInstance);
	g_hInstance = NULL;
}

BOOL App::Render()
{
	// Input ��ü, Driver ��ü
	// Render();
	if (pDxInput->Render())
	{

	}

	return pDXDriver->Render();
}

LRESULT CALLBACK App::MsgHandelr(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam)
{
	HMENU hMenu, hSubMenu;
	OPENFILENAME ofn;
	char str[300] = { 0, };
	char lpstrFile[MAX_PATH] = "";

	switch (msg)
	{
	case WM_CREATE:
		hMenu = CreateMenu();

		hSubMenu = CreatePopupMenu();
		AppendMenuW(hSubMenu, MF_STRING, ID_FILE_LOAD, TEXT("L&oad"));
		AppendMenuW(hSubMenu, MF_STRING, ID_FILE_EXIT, TEXT("E&xit"));
		AppendMenuW(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu,TEXT("&File"));

		hSubMenu = CreatePopupMenu();
		AppendMenuW(hSubMenu, MF_STRING, ID_ABOUT, TEXT("&About"));
		AppendMenuW(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Help"));

		SetMenu(hWnd, hMenu);

		break;
	case WM_KEYDOWN:
		//pDXDriver->pKey->KeyManager(hWnd, msg, wParam, IParam);
		pDxInput->pKey->KeyManager(hWnd, msg, wParam, IParam, pDXDriver);
		break;

	case WM_LBUTTONDOWN:
		pDXDriver->pMouse->MouseManager(hWnd, msg, wParam, IParam, down);
		break;

	case WM_LBUTTONUP:
		pDXDriver->pMouse->MouseManager(hWnd, msg, wParam, IParam, up);
		break;

	case WM_MOUSEMOVE:
		pDXDriver->pMouse->MouseManager(hWnd, msg, wParam, IParam, move);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			if (pDXDriver->ExitMessageBox() == IDYES)
				PostMessage(hWnd, WM_DESTROY, wParam, IParam);
			else
				return 0;
			break;

		case ID_FILE_LOAD:
			// Menu Load Click ?
			// Load Action Code
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = TEXT("Every File(*.*)\0Text File\0*.txt\0*.obj\0");
			ofn.lpstrFile = (LPWSTR)lpstrFile;
			ofn.nMaxFile = 256;
			ofn.lpstrInitialDir = TEXT("c:\\");
			if (GetOpenFileName(&ofn) != 0)
			{
				MessageBox(hWnd, (LPWSTR)(LPCTSTR)str, TEXT("���� ���� ����"), MB_OK);
			}
			break;

		case ID_ABOUT:
			MessageBox(NULL, TEXT("�����Դϴ�!"), TEXT("About"), MB_OK);
			break;
		}
		break;
	case WM_CLOSE:							// ����(X) Ŭ��
		if (pDXDriver->ExitMessageBox() == IDNO)
			return 0;

	case WM_DESTROY:
		Term();
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, msg, wParam, IParam);
	}
	return 0;
}


LRESULT CALLBACK App::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam)
{
	return MsgHandelr(hWnd, msg, wParam, IParam);
}
