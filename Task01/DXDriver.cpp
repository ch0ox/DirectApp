/*------------------------------------------------------------------------*/
//								CDxDriver.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DXDriver.h"

#include "stdafx.h"
#include "WinApp.h"
#include "main.h"
#include "KeyboardMgr.h"
#include "MouseMgr.h"
#include "Draw.h"
#include "Button.h"
#include "DXInput.h"

CDxDriver::CDxDriver()
{
	pDraw = nullptr;
	pDraw = new CDraw();
	pDraw->LinkD3D(this);

	pMouse = nullptr;
	pMouse = new CMouse();
	pMouse->LinkD3D(this);

}

CDxDriver::~CDxDriver()
{
	Term();
}


BOOL CDxDriver::Initialize(HWND hWnd)												// App 의 HWND 받아 옴.
{
	m_hWnd = hWnd;

	if (SUCCEEDED(InitD3D()))
	{
		if (SUCCEEDED(InitVB()))
		{
			if (SUCCEEDED(InitGeometry()))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

HRESULT CDxDriver::InitD3D()
{
	if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;

	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

	// 처음에 창 모드로 생성.
	m_d3dpp.Windowed = WindowMode;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		// SWAP 할 때 효율적.
	m_d3dpp.hDeviceWindow = m_hWnd;

	m_d3dpp.BackBufferFormat = mode.Format;

	if (WindowMode)
	{
		m_d3dpp.BackBufferWidth = nWidth;
		m_d3dpp.BackBufferHeight = nHeight;
	}
	else											// FULL SCREEN
	{
		m_d3dpp.BackBufferWidth = mode.Width;
		m_d3dpp.BackBufferHeight = mode.Height;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		m_d3dpp.FullScreen_RefreshRateInHz = mode.RefreshRate;
	}

	m_d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// HAL 퓨어 하드웨어 디바이스 시도
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE
		, &m_d3dpp, &m_pD3DDevice)))
	{
		// 하드웨어 랑 T&L 시도
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pD3DDevice)))
		{
			// 소프트웨어 정점
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pD3DDevice)))
			{
				return E_FAIL;
			}	// Hal - SW VP
		}	// Hal - HW VP
	}	// Hal - Pure

	return S_OK;
}


HRESULT CDxDriver::InitVB()
{
	pDraw->CreateTriangleBuffer();
	pDraw->TextInit();

	return S_OK;
}



HRESULT CDxDriver::InitGeometry()
{

	pDraw->CreateButton();
	pDraw->TriangleInit();

	for (int i = 0; i < static_cast<int>(pDraw->m_btnVector.size()); i++)
	{
		pDraw->m_btnVector[i]->CreateTexture(m_pD3DDevice);
	}

	pMouse->LinkButton(pDraw->m_btnVector);

	return S_OK;
}

VOID CDxDriver::Drawing()
{
	// Text
	pDraw->DrawTextFPS();

	// Triangle
	m_pD3DDevice->SetTexture(0, pDraw->m_pTexture);
	pDraw->SetupMatrices();
	pDraw->DrawTriangle();

	// Button
	for (int i = 0; i < static_cast<int>(pDraw->m_btnVector.size()); i++)
	{
		m_pD3DDevice->SetTexture(0, pDraw->m_btnVector[i]->m_pTexture);
		pDraw->m_btnVector[i]->SetTexture();
		pDraw->DrawRect(pDraw->m_btnVector[i]);
	}

}

VOID CDxDriver::InputRender(CDxInput* pInput)
{
	pInput->GetDevice();

	if (pInput->pKey->KeyDown(DIK_ESCAPE))
	{
		MessageBox(NULL, TEXT("ESCAPE Key Success"), TEXT("Yeahhhh"), MB_OK);
		//  수정수정~!~!
//		if (ExitMessageBox() == IDYES)
//			PostMessage(hWnd, WM_DESTROY, wParam, lParam);
//		else
//			return;
	}
	else if (pInput->pKey->KeyDown(DIK_SPACE))
	{
		MessageBox(NULL, TEXT("Space Key Success"), TEXT("Yeahhhh"), MB_OK);
	}
	else if (pInput->pKey->KeyDown(DIK_LEFT))
	{

	}
	else if (pInput->pKey->KeyDown(DIK_RIGHT))
	{

	}
}

BOOL CDxDriver::Render(CDxInput* pInput)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (m_bLostDevice)
	{
		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
		// Resource 복구 시작			 
		if (hr == D3DERR_DEVICELOST)							// Device Lost + 복구 불가능
		{
			return FALSE;
		}
		if (hr == D3DERR_DEVICENOTRESET)						// Device Lost + 복구 가능
		{
			// Reset
			HRESULT hr = m_pD3DDevice->Reset(&m_d3dpp);

			InitVB();
			InitGeometry();
			if (FAILED(hr))
			{
				MessageBox(NULL, TEXT("Device Reset Error"), TEXT("Device Error"), MB_OK);
			}

			m_bLostDevice = FALSE;
		}
	}

	// DxInput here...
	InputRender(pInput);


	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// Rendering Start 
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{


		Drawing();



		m_pD3DDevice->EndScene();
	}
	if (FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))		// Rendering 내용을 화면으로 전송.
	{
		// Lost
		m_bLostDevice = TRUE;
	}

	return TRUE;
}

VOID CDxDriver::Term()
{
	if (pMouse)
	{
		delete pMouse;
		pMouse = nullptr;
	}

	if (pDraw)
	{
		delete pDraw;
		pDraw = nullptr;
	}

	if (m_pTexture != NULL)
		SAFE_RELEASE(m_pTexture);

	if (m_pFont != NULL)
	{
		SAFE_RELEASE(m_pFont);
		m_pDesc = { NULL };
	}

	if (m_pIB != NULL)
		SAFE_RELEASE(m_pIB);

	if (m_pVB_Tri != NULL)
		SAFE_RELEASE(m_pVB_Tri);

	if (m_pVB != NULL)
		SAFE_RELEASE(m_pVB);

	if (m_pD3DDevice != NULL)
		SAFE_RELEASE(m_pD3DDevice);

	if (m_pD3D != NULL)
		SAFE_RELEASE(m_pD3D);

}


INT CDxDriver::ExitMessageBox()
{
	int result = MessageBox(NULL, (LPCWSTR)L"정말로 종료 하시겠습니까?", (LPCWSTR)L"INFO",
		MB_ICONQUESTION | MB_YESNO | MB_TOPMOST);
	return result;
}


VOID CDxDriver::ChangeDisplayMode(int mode)
{
	D3DDISPLAYMODE dmode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	int monitorX = (static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2)) - nWidth / 2;
	int monitorY = (static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 2)) - nHeight / 2;

	switch (mode)
	{
		RECT rect;
	case windowMode:
		SetRect(&rect, 0, 0, nWidth, nHeight);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, monitorX, monitorY, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		break;

	case fullScreenMode:
		SetRect(&rect, 0, 0, dmode.Width, dmode.Height);
		AdjustWindowRect(&rect, WS_VISIBLE | WS_POPUP, FALSE);

		SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		SetWindowPos(m_hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		break;
	}
	return;
}

VOID CDxDriver::DeviceLostRecovery()
{
	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	{
		ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
		m_d3dpp.Windowed = WindowMode;
		m_d3dpp.BackBufferCount = 1;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.hDeviceWindow = m_hWnd;
		m_d3dpp.BackBufferFormat = mode.Format;
		m_d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		if (WindowMode)
		{
			m_d3dpp.BackBufferWidth = nWidth;
			m_d3dpp.BackBufferHeight = nHeight;
		}
		else												// FULL SCREEN
		{
			m_d3dpp.BackBufferWidth = mode.Width;
			m_d3dpp.BackBufferHeight = mode.Height;
			m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			m_d3dpp.FullScreen_RefreshRateInHz = mode.RefreshRate;
		}

	}
}