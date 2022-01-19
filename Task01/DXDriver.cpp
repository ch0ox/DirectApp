/*------------------------------------------------------------------------*/
//								CDxDriver.cpp
/*------------------------------------------------------------------------*/


#include "DXDriver.h"

#include "main.h"
#include "WinApp.h"
#include "KeyboardMgr.h"
#include "MouseMgr.h"
#include "Draw.h"
#include "Button.h"
#include "ObjMgr.h"
#include "Timer.h"
#include "DXInput.h"

CDxDriver::CDxDriver(App* app)	:	m_pApp(app)
{
	m_pObjMgr = nullptr;
	m_pObjMgr = new CObjMgr();

	m_pDraw = nullptr;
	m_pDraw = new CDraw();
	m_pDraw->LinkD3D(this);

	m_pMouse = nullptr;
	m_pMouse = new CMouse();
	m_pMouse->LinkD3D(this);
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
	m_pDraw->CreateTriangleBuffer();
	m_pDraw->TextInit();

	// After Obj file Load (for Toggle )
	if (m_pApp->m_bObjLoad)
	{
		m_pApp->GetObjMgr()->CreateObjBuffer(this);
	}
	
	return S_OK;
}



HRESULT CDxDriver::InitGeometry()
{
	m_pDraw->CreateButton();
	//m_pDraw->TriangleInit();

	for (int i = 0; i < static_cast<int>(m_pDraw->m_btnVector.size()); i++)
	{
		m_pDraw->m_btnVector[i]->CreateTexture(m_pD3DDevice);
	}

	m_pMouse->LinkButton(m_pDraw->m_btnVector);

	return S_OK;
}

VOID CDxDriver::Drawing()
{
	// Text
	m_pDraw->SetDuringTime(static_cast<float>(m_pApp->GetTimer()->GetFPS()));
	m_pDraw->DrawTextFPS();

	// Button
	for (int i = 0; i < static_cast<int>(m_pDraw->m_btnVector.size()); i++)
	{
		m_pD3DDevice->SetTexture(0, m_pDraw->m_btnVector[i]->m_pTexture);
		m_pDraw->m_btnVector[i]->SetTexture();
		m_pDraw->DrawRect(m_pDraw->m_btnVector[i]);
	}

	// Triangle
//m_pD3DDevice->SetTexture(0, m_pDraw->m_pTexture);
//m_pDraw->SetupMatrices();
//m_pDraw->DrawTriangle();


	// Object Model
	if (m_pApp->m_bObjLoad)		// obj file 이 Load 된 후에만 Drawing.
	{
		m_pApp->GetObjMgr()->ObjDraw(this);
	}
}


VOID CDxDriver::InputRender(CDxInput* pInput)
{
	pInput->GetDevice();

	// KeyBoard
	if (pInput->pKey->KeyDown(DIK_ESCAPE))
	{
		if (ExitMessageBox() == IDYES)
			PostMessage(pInput->GetHwnd(), WM_DESTROY, 0, 0);
		else
			return;
	}
	else if (pInput->pKey->KeyDown(DIK_SPACE))
	{
		MessageBox(NULL, TEXT("Space Key Success"), TEXT("Yeahhhh"), MB_OK);
		pInput->pKey->Toggle(this);
	}
	else if (pInput->pKey->KeyDown(DIK_LEFT))
	{

	}
	else if (pInput->pKey->KeyDown(DIK_RIGHT))
	{

	}

	// Mouse
	POINT pt;
	pt = pInput->m_pMouse->ClientCursorPos(m_hWnd);
}

BOOL CDxDriver::Render(CDxInput* pInput)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	// Timer
	m_pApp->GetTimer()->Tick(60.0f);

	if (m_bLostDevice)
	{
		HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
		// Resource 복구 시작			 
		if (hr == D3DERR_DEVICELOST)							// Device Lost + 복구 불가능
		{
			return FALSE;
		}
		else if (hr == D3DERR_DEVICENOTRESET)						// Device Lost + 복구 가능
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

HRESULT CDxDriver::SetTexture(UINT texture)
{
	if (texture == 0)
	{
		return m_pD3DDevice->SetTexture(0, nullptr);
	}
	return m_pD3DDevice->SetTexture(0, m_pTextureList[texture]);
}

VOID CDxDriver::Term()
{
	if (m_pMouse)
	{
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	if (m_pDraw)
	{
		delete m_pDraw;
		m_pDraw = nullptr;
	}

	if (m_pObjMgr)
	{
// 		delete m_pObjMgr;
// 		m_pObjMgr = nullptr;
	}

	for (int i = 0; i < m_pTextureList.size(); i++)
	{
		SAFE_RELEASE(m_pTextureList[i]);
	}
	m_pTextureList.clear();

	for (int i = 0; i < m_pIndexBufferList.size(); i++)
	{
		SAFE_RELEASE(m_pIndexBufferList[i]);
	}
	m_pIndexBufferList.clear();

	for (int i = 0; i < m_pVertexBufferList.size(); i++)
	{
		SAFE_RELEASE(m_pVertexBufferList[i]);
	}
	m_pVertexBufferList.clear();

	if (m_pTexture != nullptr)
		SAFE_RELEASE(m_pTexture);

	if (m_pFont != nullptr)
	{
		SAFE_RELEASE(m_pFont);
		m_pDesc = { NULL };
	}

	if (m_pIB != nullptr)
		SAFE_RELEASE(m_pIB);

	if (m_pVB_Tri != nullptr)
		SAFE_RELEASE(m_pVB_Tri);

	if (m_pVB != nullptr)
		SAFE_RELEASE(m_pVB);

	if (m_pD3DDevice != nullptr)
		SAFE_RELEASE(m_pD3DDevice);

	if (m_pD3D != nullptr)
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

// Obj 의 정보를 가지고 Create Buffers
UINT CDxDriver::CreateObjVertexBuffer(UINT length, DWORD usage,DWORD fvf, D3DPOOL pool)
{
	UINT index = (UINT)-1;

	if (m_pD3DDevice == nullptr)
	{
		MessageBox(NULL, TEXT("There's no d3ddevice."), TEXT("Error"), MB_OK);
		return index;
	}

	LPDIRECT3DVERTEXBUFFER9 buffer = nullptr;
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(length, usage, fvf, pool, &buffer, nullptr)))
	{
		MessageBox(NULL, TEXT("Obj Create Vertex Buffer Error"), TEXT("Error"), MB_OK);
		return index;
	}

	index = (UINT)m_pVertexBufferList.size();
	m_pVertexBufferList.push_back(buffer);

	return index;
}

UINT CDxDriver::CreateObjIndexBuffer(UINT length, DWORD usage, D3DFORMAT format, D3DPOOL pool)
{
	UINT index = -1;
	if (m_pD3DDevice == nullptr)
	{
		MessageBox(NULL, TEXT("There's no d3ddevice."), TEXT("Error"), MB_OK);
		return index;
	}

	LPDIRECT3DINDEXBUFFER9 buffer = nullptr;
	if (FAILED(m_pD3DDevice->CreateIndexBuffer(length, usage, format, pool, &buffer, nullptr)))
		return index;

	index = (UINT)m_pIndexBufferList.size();
	m_pIndexBufferList.push_back(buffer);

	return index;
}

HRESULT CDxDriver::CopyObjVertexBuffer(UINT index, const void* p_src, int p_size)
{
	if (m_pD3DDevice == nullptr)
	{
		MessageBox(NULL, TEXT("There's no d3ddevice."), TEXT("Error"), MB_OK);
		return E_FAIL;
	}

	if (index == -1 || index >= m_pVertexBufferList.size())
		return E_FAIL;

	VOID* pVertices;
	m_pVertexBufferList[index]->Lock(0, 0, &pVertices, 0);
	memcpy(pVertices, p_src, p_size);
	m_pVertexBufferList[index]->Unlock();

	return S_OK;
}


HRESULT CDxDriver::CopyObjIndexBuffer(UINT index, const void* p_src, int p_size)
{
	if (m_pD3DDevice == nullptr)
	{
		MessageBox(NULL, TEXT("There's no d3ddevice."), TEXT("Error"), MB_OK);
		return E_FAIL;
	}

	if (index == -1 || index >= m_pVertexBufferList.size())
		return E_FAIL;

	VOID* pIndices;
	m_pIndexBufferList[index]->Lock(0, 0, &pIndices, 0);
	memcpy(pIndices, p_src, p_size);
	m_pIndexBufferList[index]->Unlock();



	return S_OK;
}

// If Obj Model is Triangle_List Mode, 
VOID CDxDriver::DrawObjListModel(CObjMgr* pObjMgr)
{
	DWORD dwFVF;

	for (int i = 0; i < pObjMgr->m_verticesList.size(); i++)
	{
		if (pObjMgr->m_bIsTexturingList[i])
			dwFVF = D3DFVF_TEXTUREVERTEX;
		else
			dwFVF = D3DFVF_NOTEXTUREVERTEX;

		// For Test
//		if ((pObjMgr->m_list_indicesList[i].size() / 3) == pObjMgr->m_primCountList[i])
//		{
//			MessageBox(NULL, TEXT("prim 동일"), TEXT("Success"), MB_OK);
//		}
//		else
//			MessageBox(NULL, TEXT("prim 다름"), TEXT("Failed"), MB_OK);

		m_pD3DDevice->SetStreamSource(i, m_pVertexBufferList[i], 0, sizeof(OBJVERTEX));
		m_pD3DDevice->SetFVF(dwFVF);
		m_pD3DDevice->SetIndices(m_pIndexBufferList[i]);
		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, i, 0,
														pObjMgr->m_verticesList[i].size(), 0,
													  //pObjMgr->m_primCountList[i]);				// PrimitiveCount : Triangle Count
														pObjMgr->m_list_indicesList[i].size() / 3);
		// For Cube Test
//		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		if (FAILED(hr))
			MessageBox(NULL, TEXT("DrawIndexedPrimitive Error"), TEXT("DrawIndexedPrimitive Error"), MB_OK);
//		else
//			MessageBox(NULL, TEXT("DrawIndexedPrimitive Success"), TEXT("Success"), MB_OK);
	}
}

// If Obj Model is Triangle_Strip Mode, 
VOID CDxDriver::DrawObjStripModel(CObjMgr* pObjMgr)
{
	DWORD dwFVF;

	for (int i = 0; i < pObjMgr->m_verticesList.size(); i++)
	{
		if (pObjMgr->m_bIsTexturingList[i])
			dwFVF = D3DFVF_TEXTUREVERTEX;
		else
			dwFVF = D3DFVF_NOTEXTUREVERTEX;

		m_pD3DDevice->SetFVF(dwFVF);
		m_pD3DDevice->SetStreamSource(i, m_pVertexBufferList[i], 0, sizeof(OBJVERTEX));
		m_pD3DDevice->SetIndices(m_pIndexBufferList[i]);
		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
														pObjMgr->m_verticesList[i].size(), 0,
													  //pObjMgr->m_primCountList[i]);				// PrimitiveCount : Triangle Count
														pObjMgr->m_list_indicesList[i].size() / 3);
		if (FAILED(hr))
			MessageBox(NULL, TEXT("DrawIndexedPrimitive Error"), TEXT("DrawIndexedPrimitive Error"), MB_OK);
	}
}

VOID CDxDriver::SetWorldMatrix(D3DXMATRIXA16& matWorld)
{
	FLOAT m_fAngle = 0.0f;
	FLOAT m_fScale = 1.0f;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matWorld, 0.0f, 10.0f, -15.0f);
	D3DXMatrixRotationY(&matWorld, m_fAngle);
	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matWorld)))
		MessageBox(NULL, TEXT("Set World Matrix Error"), TEXT("Matrix Error"), MB_OK);
//	else
//		MessageBox(NULL, TEXT("Set World Matrix Success"), TEXT("Success"), MB_OK);
}

VOID CDxDriver::SetCameraMatrix(D3DXMATRIXA16& matView, D3DXVECTOR3 p_eye, D3DXVECTOR3 p_at, D3DXVECTOR3 p_up)
{
	D3DXMatrixIdentity(&matView);
	D3DXMatrixLookAtLH(&matView, &p_eye, &p_at, &p_up);	// Camera 변환 행렬 계산
	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, &matView)))
		MessageBox(NULL, TEXT("Set VIEW Matrix Error"), TEXT("Matrix Error"), MB_OK);
//	else
//		MessageBox(NULL, TEXT("Set VIEW Matrix Success"), TEXT("Success"), MB_OK);
}

VOID CDxDriver::SetProjMatrix(D3DXMATRIXA16& matProj)		// 원근행렬
{
	D3DXMatrixIdentity(&matProj);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 0.5f, 1.0f, 1.0f, 1000.f);
	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj)))
		MessageBox(NULL, TEXT("Set PROJ Matrix Error"), TEXT("Matrix Error"), MB_OK);
//	else
//		MessageBox(NULL, TEXT("Set PROJ Matrix Success"), TEXT("Success"), MB_OK);
}


VOID CDxDriver::SetLight()
{
	if(m_pD3DDevice == nullptr)
		MessageBox(NULL, TEXT("No D3DDevice"), TEXT("Set Error"), MB_OK);

	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));

	light.Ambient.r = light.Diffuse.g = light.Diffuse.b = 0.5f;			// 환경광
	light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = 0.5f;			// 난반사광
	light.Direction.x = 0.0f;
	light.Direction.y = 0.0f;
	light.Direction.z = 1.0f;

	// Light Enroll.
	m_pD3DDevice->SetLight(0, &light);
	m_pD3DDevice->LightEnable(0, TRUE);

}

VOID CDxDriver::SetMaterial()
{
	D3DMATERIAL9 material;
	memset(&material, 0, sizeof(material));
	material.Diffuse.r = material.Diffuse.g = material.Diffuse.b = 1.0f;
	material.Ambient.r = material.Ambient.g = material.Ambient.b = 1.0f;

	// Material Enroll.
	m_pD3DDevice->SetMaterial(&material);
}


