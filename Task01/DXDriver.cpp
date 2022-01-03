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
	g_hWnd = hWnd;

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
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;

	D3DDISPLAYMODE mode;
	pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	// 처음에 창 모드로 생성.
	d3dpp.Windowed = WindowMode;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		// SWAP 할 때 효율적.
	d3dpp.hDeviceWindow = g_hWnd;

	d3dpp.BackBufferFormat = mode.Format;

	if (WindowMode)
	{
		d3dpp.BackBufferWidth = nWidth;
		d3dpp.BackBufferHeight = nHeight;
	}
	else											// FULL SCREEN
	{
		d3dpp.BackBufferWidth = mode.Width;
		d3dpp.BackBufferHeight = mode.Height;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		d3dpp.FullScreen_RefreshRateInHz = mode.RefreshRate;
	}

	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// HAL 퓨어 하드웨어 디바이스 시도
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE
		, &d3dpp, &pd3dDevice)))
	{
		// 하드웨어 랑 T&L 시도
		if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice)))
		{
			// 소프트웨어 정점
			if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice)))
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

	for (int i = 0; i < static_cast<int>(pDraw->btnVector.size()); i++)
	{
		pDraw->btnVector[i]->CreateTexture(pd3dDevice);
	}

	pMouse->LinkButton(pDraw->btnVector);

	return S_OK;
}

VOID CDxDriver::Drawing()
{
	// Text
	pDraw->DrawTextFPS();

	// Triangle
	pd3dDevice->SetTexture(0, pDraw->m_pTexture);
	pDraw->SetupMatrices();
	pDraw->DrawTriangle();

	// Button
	for (int i = 0; i < static_cast<int>(pDraw->btnVector.size()); i++)
	{
		pd3dDevice->SetTexture(0, pDraw->btnVector[i]->m_pTexture);
		pDraw->btnVector[i]->SetTexture();
		pDraw->DrawRect(pDraw->btnVector[i]);
	}

}

BOOL CDxDriver::Render()
{
	if (pd3dDevice == NULL)
		return FALSE;

	if (g_bLostDevice)
	{
		HRESULT hr = pd3dDevice->TestCooperativeLevel();
		// Resource 복구 시작			 
		if (hr == D3DERR_DEVICELOST)							// Device Lost + 복구 불가능
		{
			return FALSE;
		}
		if (hr == D3DERR_DEVICENOTRESET)						// Device Lost + 복구 가능
		{
			// Reset
			HRESULT hr = pd3dDevice->Reset(&d3dpp);

			InitVB();
			InitGeometry();
			if (FAILED(hr))
			{
				MessageBox(NULL, TEXT("Device Reset Error"), TEXT("Device Error"), MB_OK);
			}

			g_bLostDevice = FALSE;
		}
	}

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// Rendering Start 
	if (SUCCEEDED(pd3dDevice->BeginScene()))
	{


		Drawing();



		pd3dDevice->EndScene();
	}
	if (FAILED(pd3dDevice->Present(NULL, NULL, NULL, NULL)))		// Rendering 내용을 화면으로 전송.
	{
		// Lost
		g_bLostDevice = TRUE;
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

	if (g_pTexture != NULL)
		SAFE_RELEASE(g_pTexture);

	if (g_pFont != NULL)
	{
		SAFE_RELEASE(g_pFont);
		g_pDesc = { NULL };
	}

	if (g_pIB != NULL)
		SAFE_RELEASE(g_pIB);

	if (g_pVB_Tri != NULL)
		SAFE_RELEASE(g_pVB_Tri);

	if (g_pVB != NULL)
		SAFE_RELEASE(g_pVB);

	if (pd3dDevice != NULL)
		SAFE_RELEASE(pd3dDevice);

	if (pD3D != NULL)
		SAFE_RELEASE(pD3D);

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
	pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	int monitorX = (static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2)) - nWidth / 2;
	int monitorY = (static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 2)) - nHeight / 2;

	switch (mode)
	{
		RECT rect;
	case windowMode:
		SetRect(&rect, 0, 0, nWidth, nHeight);
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		SetWindowLong(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(g_hWnd, HWND_NOTOPMOST, monitorX, monitorY, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		break;

	case fullScreenMode:
		SetRect(&rect, 0, 0, dmode.Width, dmode.Height);
		AdjustWindowRect(&rect, WS_VISIBLE | WS_POPUP, FALSE);

		SetWindowLong(g_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		SetWindowPos(g_hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		break;
	}
	return;
}

VOID CDxDriver::DeviceLostRecovery()
{
	D3DDISPLAYMODE mode;
	pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	{
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = WindowMode;
		d3dpp.BackBufferCount = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = g_hWnd;
		d3dpp.BackBufferFormat = mode.Format;
		d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		if (WindowMode)
		{
			d3dpp.BackBufferWidth = nWidth;
			d3dpp.BackBufferHeight = nHeight;
		}
		else												// FULL SCREEN
		{
			d3dpp.BackBufferWidth = mode.Width;
			d3dpp.BackBufferHeight = mode.Height;
			d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			d3dpp.FullScreen_RefreshRateInHz = mode.RefreshRate;
		}

	}
}