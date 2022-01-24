/*------------------------------------------------------------------------*/
//								WinApp.cpp
/*------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*/
//								INCLUDES
/*------------------------------------------------------------------------*/


#include "WinApp.h"
#include "main.h"
#include "KeyboardMgr.h"
#include "MouseMgr.h"
#include "Draw.h"
#include "Menu.h"
#include "ButtonMgr.h"
#include "Timer.h"
#include "ObjMgr.h"
#include <fstream>


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

	m_pDxInput = new CDxInput();						// Input 객체
	if (!m_pDxInput)
		return FALSE;

	m_pDXDriver = new CDxDriver(this);					// Driver 객체
	if (!m_pDXDriver)
		return FALSE;

	m_pButtonMgr = new CButtonMgr();
	if (!m_pButtonMgr)
		return FALSE;

	m_pTimer = new CTimer();
	if (!m_pTimer)
		return FALSE;

	m_pObjMgr = new CObjMgr();
	if (!m_pObjMgr)
		return FALSE;

	//m_pObjModel->AddObjModel(m_pDXDriver);

	m_pDxInput->Initialize(g_hInstance, m_hWnd);
	if (!m_pDXDriver->Initialize(m_hWnd))
	{
		MessageBox(NULL, TEXT("DX Initialize Error"), TEXT("DX Initialize Error"), MB_OK);
		return FALSE;
	}

	if (!m_pButtonMgr->LoadIniFile("\\\\ifs01\\서든어택1실\\SA_Depts\\클라이언트팀\\99. 공유\\김채원\\Study_Git\\directApp\\button.ini"))
	{
		MessageBox(NULL, TEXT("Btn Load Error"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	else
	{
		m_pButtonMgr->Initialize(m_pDXDriver);
	}

	m_pDXDriver->SetLight(TRUE);
	m_pDXDriver->SetMaterial();



	return TRUE;
}

VOID App::Term()
{
	if (m_pObjModel)
	{
		delete m_pObjModel;
		m_pObjModel = nullptr;
	}

	if (m_pObjMgr)
	{
		delete m_pObjMgr;
		m_pObjMgr = nullptr;
	}

	if (m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = nullptr;
	}

	if (m_pButtonMgr)
	{
		delete m_pButtonMgr;
		m_pButtonMgr = nullptr;
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

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
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

	// Menu Bar 를 만들면서 조금 밀렸음. -> 수정완료
	RECT rect;
	SetRect(&rect, 0, 0, nWidth, nHeight);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

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
	if (m_pDXDriver)
	{
		return m_pDXDriver->Render(m_pDxInput);
	}
}

VOID App::FileLoad(HWND hWnd)
{
	OPENFILENAME ofn;
	char lpstrFile[MAX_PATH] = "";

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"All File(*.*)\0*.*\0Text File(.txt)\0*.txt\0Obj File(.obj)\0*.obj\0";
	ofn.lpstrFile = (LPWSTR)lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = TEXT("c:\\");


	if (GetOpenFileName(&ofn) != 0)
	{
		std::string str;
		std::wstring value = ofn.lpstrFile;
		str.assign(value.begin(), value.end());

		std::string filepath = str;
		std::ifstream file(filepath);
		MessageBox(hWnd, ofn.lpstrFile, TEXT("파일 열기 성공"), MB_OK);

		if (!m_pObjMgr->ObjLoad(file, m_pDXDriver))
		{
			MessageBox(hWnd, TEXT("Obj Load Failed!"), TEXT("Error"), MB_OK);
		}
		else
		{
			m_bObjLoad = TRUE;
		}
		m_pDXDriver->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1.0f));

		file.close();
	}
}


LRESULT CALLBACK App::MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam)
{
	HMENU hMenu, hSubMenu;

	switch (msg)
	{
	case WM_CREATE:
		hMenu = CreateMenu();

		hSubMenu = CreatePopupMenu();
		AppendMenuW(hSubMenu, MF_STRING, ID_FILE_LOAD, TEXT("L&oad"));
		AppendMenuW(hSubMenu, MF_STRING, ID_FILE_EXIT, TEXT("E&xit"));
		AppendMenuW(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&File"));

		hSubMenu = CreatePopupMenu();
		AppendMenuW(hSubMenu, MF_STRING, ID_ABOUT, TEXT("&About"));
		AppendMenuW(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Help"));

		SetMenu(hWnd, hMenu);

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
			FileLoad(hWnd);
			break;

		case ID_ABOUT:
			MessageBox(NULL, TEXT("김채원\n과제입니당!"), TEXT("About"), MB_OK);
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
