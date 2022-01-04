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
#include "Button.h"
#include "Timer.h"


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
	InitWindow();									// Window 생성 초기화

	m_pDxInput = new CDxInput;						// Input 객체
	if (!m_pDxInput)
		return FALSE;

	m_pDXDriver = new CDxDriver;					// Driver 객체
	if (!m_pDXDriver)
		return FALSE;

	m_pButton = new CButton;
	if (!m_pButton)
		return FALSE;

	m_pTimer = new CTimer;
	if (!m_pTimer)
		return FALSE;


	m_pDxInput->Initialize(g_hInstance, m_hWnd);
	if (!m_pDXDriver->Initialize(m_hWnd))
	{
		MessageBox(NULL, TEXT("DX Initialize Error"), TEXT("DX Initialize Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

VOID App::Term()
{
	if (m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = nullptr;
	}

	if (m_pButton)
	{
		delete m_pButton;
		m_pButton = nullptr;
	}

	if (m_pDXDriver)
	{
		delete m_pDXDriver;							// Driver 객체 반환
		m_pDXDriver = nullptr;
	}

	if (m_pDxInput)
	{
		delete m_pDxInput;							// Input 객체 반환
		m_pDxInput = nullptr;
	}

	ShutDownWindow();								// Window 종료
}

VOID App::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));									// msg init

	while (msg.message != WM_QUIT)
	{
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);								// SW_SHOWMAXIMIZED  SW_SHOWMINIMIZED
		UpdateWindow(m_hWnd);
		//SetFocus(m_hWnd);

		QueryPerformanceFrequency(&m_frequency);							// CPU 주파수에 따른 1초당 진행되는 틱 수 (10000000)
	//	m_fps = (double)m_frequency.QuadPart / FPS;						// 초당 frame 간격 설정

		QueryPerformanceCounter(&m_lastTime);								// 시작 시점


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

			QueryPerformanceCounter(&m_currentTime);											// 끝 시점 (현재)
			clocks = static_cast<double>(m_currentTime.QuadPart - m_lastTime.QuadPart);			// 클럭 수

			duringTime = clocks / (double)m_frequency.QuadPart * 1000.0;
			//		duringTime = clocks / (m_fps) * 1000.0;											// 초 단위 수행시간 * FPS
			if (m_pDXDriver != nullptr)
			{
				m_pDXDriver->pDraw->SetDuringTime(static_cast<float>(duringTime));
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

	m_hWnd = ::CreateWindowEx(0, TEXT("Task"), TEXT("Task"), WS_OVERLAPPEDWINDOW, monitorX, monitorY, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, ::GetModuleHandle(NULL), NULL);
	DWORD d = GetLastError();

	return;
}

VOID App::ShutDownWindow()
{
	m_hWnd = NULL;

	UnregisterClass(L"Task", g_hInstance);
	g_hInstance = NULL;
}

BOOL App::Render()
{
	// Input 객체, Driver 객체
	// Render.
	if (m_pDxInput->Render())
	{

	}

	return m_pDXDriver->Render();
}

LRESULT CALLBACK App::MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam)
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
		//m_pDXDriver->pKey->KeyManager(hWnd, msg, wParam, IParam);
		m_pDxInput->pKey->KeyManager(hWnd, msg, wParam, IParam, m_pDXDriver);
		break;

	case WM_LBUTTONDOWN:
		m_pDXDriver->pMouse->MouseManager(hWnd, msg, wParam, IParam, down);
		break;

	case WM_LBUTTONUP:
		m_pDXDriver->pMouse->MouseManager(hWnd, msg, wParam, IParam, up);
		break;

	case WM_MOUSEMOVE:
		m_pDXDriver->pMouse->MouseManager(hWnd, msg, wParam, IParam, move);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_EXIT:
			if (m_pDXDriver->ExitMessageBox() == IDYES)
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
				MessageBox(hWnd, (LPWSTR)(LPCTSTR)str, TEXT("파일 열기 성공"), MB_OK);
			}
			break;

		case ID_ABOUT:
			MessageBox(NULL, TEXT("과제입니당!"), TEXT("About"), MB_OK);
			break;
		}
		break;
	case WM_CLOSE:							// 종료(X) 클릭
		if (m_pDXDriver->ExitMessageBox() == IDNO)
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
	return MsgHandler(hWnd, msg, wParam, IParam);
}
