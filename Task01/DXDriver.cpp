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

CDxDriver::CDxDriver(App* app) : m_pApp(app)
{
	m_pDraw = nullptr;
	m_pDraw = new CDraw();

	m_eye.x = 0.0f;
	m_eye.y = 2.0f;
	m_eye.z = -5.0f;

	m_at.x = 0.0f;
	m_at.y = 0.0f;
	m_at.z = 0.0f;

	m_up.x = 0.0f;
	m_up.y = 1.0f;
	m_up.z = 0.0f;
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
			m_pTextureList.push_back(nullptr);
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
	// Triangle
	m_pDraw->CreateTriangleBuffer(this);

	m_pDraw->TextInit(this);

	// After Obj file Load ( for Toggle )
	if (m_pApp->m_bObjLoad)
	{
		//m_pApp->GetObjMgr()->CreateObjBuffer(this);
		m_pApp->GetObjMgr()->CreateOriginalObjBuffer(this);
	}

	return S_OK;
}

HRESULT CDxDriver::InitGeometry()
{
	//m_pTextureList.push_back(m_pDraw->m_pTexture);

	return S_OK;
}

VOID CDxDriver::Drawing()
{
	// Text
	m_pDraw->SetDuringTime(static_cast<float>(m_pApp->GetTimer()->GetFPS()));
	m_pDraw->DrawTextFPS(this);

	// button
	m_pApp->GetButtonMgr()->Draw(this);

	// Matrix
	SetWorldMatrix(m_matWorld);
	SetCameraMatrix(m_matView, m_eye, m_at, m_up);
	SetProjMatrix(m_matProj);

	// Object Model
	if (m_pApp->m_bObjLoad)		// obj file 이 Load 된 후에만 Drawing.
	{
		m_pApp->GetObjMgr()->ObjDraw(this);
	}
	// Triangle
	else
	{
		m_pD3DDevice->SetTexture(0, m_pDraw->m_pTexture);
		m_pDraw->DrawTriangle(this);
	}


}


VOID CDxDriver::InputRender(CDxInput* pInput)
{
	pInput->GetDevice();
	D3DXVECTOR3 p_eye = GetEye();
	D3DXVECTOR3 p_at = GetAt();
	D3DXVECTOR3 p_up = GetUp();

	if (!pInput->m_pKey)
		return;
	if (!pInput->m_pMouse)
		return;

	// KeyBoard
	if (pInput->m_pKey->IsKeyDown(DIK_A))		// <-
	{
		p_eye.x += 0.02f;
		p_at.x += 0.01f;
	}
	else if (pInput->m_pKey->IsKeyDown(DIK_D))	// ->
	{
		p_eye.x -= 0.02f;
		p_at.x -= 0.01f;
	}
	else if (pInput->m_pKey->IsKeyDown(DIK_S))
	{
		p_eye.z += 0.02f;
		p_at.z -= 0.001f;
	}
	else if (pInput->m_pKey->IsKeyDown(DIK_W))
	{
		p_eye.z -= 0.02f;
		p_at.z += 0.01f;
	}
	else if (pInput->m_pKey->IsKeyDown(DIK_ESCAPE))
	{
		if (ExitMessageBox() == IDYES)
			PostMessage(pInput->GetHwnd(), WM_DESTROY, 0, 0);
		else
			return;
	}
	else if (pInput->m_pKey->IsKeyDown(DIK_SPACE))
	{
		MessageBox(NULL, TEXT("Space Key Success"), TEXT("Yeahhhh"), MB_OK);
		pInput->m_pKey->Toggle(this);
	}

	SetEye(p_eye);
	SetAt(p_at);
	SetUp(p_up);


	// Mouse
	POINT pt;
	pt = pInput->m_pMouse->ClientCursorPos(m_hWnd);

	m_pApp->GetButtonMgr()->Check(this, pInput, pInput->m_pMouse->IsMouseDown(0), pt.x, pt.y);

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

	if (pInput)
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
	if (texture == -1)
	{
		MessageBox(NULL, TEXT("Texture 실패"), TEXT("Error"), MB_OK);
		return m_pD3DDevice->SetTexture(0, nullptr);
	}
	return m_pD3DDevice->SetTexture(0, m_pTextureList[texture]);
}

UINT CDxDriver::CreateTexture(const char* psz)
{
	UINT texture = -1;
	LPDIRECT3DTEXTURE9 pTex = nullptr;

	if (FAILED(D3DXCreateTextureFromFileA(m_pD3DDevice, psz, &pTex)))
	{
		MessageBox(NULL, TEXT("CreateTexture Failed!"), TEXT("Error"), MB_OK);
		return texture;
	}

	texture = (UINT)m_pTextureList.size();
	m_pTextureList.push_back(pTex);

	return texture;
}

VOID CDxDriver::Term()
{
	if (m_pDraw)
	{
		delete m_pDraw;
		m_pDraw = nullptr;
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

	if (m_pFont != nullptr)
	{
		SAFE_RELEASE(m_pFont);
		m_pDesc = { NULL };
	}

	if (m_pIB_Tri != nullptr)
		SAFE_RELEASE(m_pIB_Tri);

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
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

		SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, monitorX, monitorY, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
		break;

	case fullScreenMode:
		SetRect(&rect, 0, 0, dmode.Width, dmode.Height);
		AdjustWindowRect(&rect, WS_VISIBLE | WS_POPUP, TRUE);

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
UINT CDxDriver::CreateObjVertexBuffer(UINT length, DWORD usage, DWORD fvf, D3DPOOL pool)
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
	m_pVertexBufferList[index]->Lock(0, p_size, (VOID**)&pVertices, 0);
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
	m_pIndexBufferList[index]->Lock(0, p_size, (VOID**)&pIndices, 0);
	memcpy(pIndices, p_src, p_size);
	m_pIndexBufferList[index]->Unlock();



	return S_OK;
}

VOID CDxDriver::DrawObjOriginalModel(CObjMgr* pObjMgr)
{
	DWORD dwFVF;

	if (pObjMgr->GetObjTex())
	{
		for (int i = 0; i < pObjMgr->m_mtls.size(); i++)
		{
			if (pObjMgr->m_mtls[i].texIndex != (UINT)-1)
				SetTexture(pObjMgr->m_mtls[i].texIndex);
			else
				SetTexture(NO_TEXTURE);
		}
	}
	else
		SetTexture(NO_TEXTURE);

	for (int i = 0; i < m_pVertexBufferList.size(); i++)
	{
		if (pObjMgr->m_bIsTexturingList[i])
		{
			dwFVF = D3DFVF_TEXTUREVERTEX;

		}

		else
		{
			dwFVF = D3DFVF_NOTEXTUREVERTEX;
			SetTexture(NO_TEXTURE);
		}


		// Drawing Preparation
		m_pD3DDevice->SetStreamSource(0, m_pVertexBufferList[i], 0, sizeof(OBJVERTEX));
		m_pD3DDevice->SetFVF(dwFVF);

		m_pD3DDevice->SetIndices(m_pIndexBufferList[i]);
		// Drawing
		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			pObjMgr->m_original_V_List[i].size(), 0,
			pObjMgr->m_original_I_List[i].size() / 3);

		if (FAILED(hr))
			MessageBox(NULL, TEXT("DrawIndexedPrimitive Error"), TEXT("DrawIndexedPrimitive Error"), MB_OK);
		//		else
		//			MessageBox(NULL, TEXT("DrawIndexedPrimitive Success"), TEXT("Success"), MB_OK);
	}
}

// If Obj Model is Triangle_List Mode, 
VOID CDxDriver::DrawObjListModel(CObjMgr* pObjMgr)
{
	DWORD dwFVF;

	if (pObjMgr->GetObjTex())
	{
		for (int i = 0; i < pObjMgr->m_mtls.size(); i++)
		{
			if (pObjMgr->m_mtls[i].texIndex != (UINT)-1)
				SetTexture(pObjMgr->m_mtls[i].texIndex);
			else
				SetTexture(0);
		}
	}
	else
		SetTexture(0);

	for (int i = 0; i < m_pVertexBufferList.size(); i++)
	{
		if (pObjMgr->m_bIsTexturingList[i])
		{
			dwFVF = D3DFVF_TEXTUREVERTEX;
			
		}
			
		else
		{
			dwFVF = D3DFVF_NOTEXTUREVERTEX;
			SetTexture(0);
		}


		// Drawing Preparation
		m_pD3DDevice->SetStreamSource(0, m_pVertexBufferList[i], 0, sizeof(OBJVERTEX));
		m_pD3DDevice->SetFVF(dwFVF);

		m_pD3DDevice->SetIndices(m_pIndexBufferList[i]);
		// Drawing
		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			pObjMgr->m_verticesList[i].size(), 0,
			pObjMgr->m_list_indicesList[i].size() / 3);

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

	for (int i = 0; i < m_pVertexBufferList.size(); i++)
	{
		if (pObjMgr->m_bIsTexturingList[i])
			dwFVF = D3DFVF_TEXTUREVERTEX;
		else
		{
			dwFVF = D3DFVF_NOTEXTUREVERTEX;
			//SetTexture(NO_TEXTURE);
		}

		m_pD3DDevice->SetFVF(dwFVF);
		m_pD3DDevice->SetStreamSource(i, m_pVertexBufferList[i], 0, sizeof(OBJVERTEX));
		m_pD3DDevice->SetIndices(m_pIndexBufferList[i]);
		HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,
			pObjMgr->m_verticesList[i].size(), 0,
			pObjMgr->m_list_indicesList[i].size() / 3);
		if (FAILED(hr))
			MessageBox(NULL, TEXT("DrawIndexedPrimitive Error"), TEXT("DrawIndexedPrimitive Error"), MB_OK);
	}
}

VOID CDxDriver::SetWorldMatrix(D3DXMATRIXA16& matWorld)
{
	D3DXMatrixIdentity(&matWorld);
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
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 3.0f, 1.0f, 1.0f, 1000.f);
	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj)))
		MessageBox(NULL, TEXT("Set PROJ Matrix Error"), TEXT("Matrix Error"), MB_OK);
	//	else
	//		MessageBox(NULL, TEXT("Set PROJ Matrix Success"), TEXT("Success"), MB_OK);
}


VOID CDxDriver::SetLight(BOOL bLight)
{
	if (m_pD3DDevice == nullptr)
		MessageBox(NULL, TEXT("No D3DDevice"), TEXT("Set Error"), MB_OK);

	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));

	light.Type = D3DLIGHT_DIRECTIONAL;

	light.Ambient.r = light.Ambient.g = light.Ambient.b = 0.8f;			// 환경광
	light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = 0.5f;			// 난반사광
	light.Direction.x = 0.0f;
	light.Direction.y = 0.0f;
	light.Direction.z = 2.0f;
	light.Range = 10.0f;

	// Light Enroll.
	m_pD3DDevice->SetLight(0, &light);
	m_pD3DDevice->LightEnable(0, bLight);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, bLight);
	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

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

VOID CDxDriver::SetPosition(D3DXVECTOR3 pos)
{
	D3DXMatrixTranslation(&m_matWorld, pos.x, pos.y, pos.z);
}

// Use D3DFVF_XYZRHW and DrawPrimitiveUp.
VOID CDxDriver::DrawRect(CButton* pButton)
{
	m_pD3DDevice->SetFVF(D3DFVF_BOXVERTEX);
	HRESULT hr = m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pButton->m_vertex, sizeof(RHWVERTEX));

	if (FAILED(hr))
		MessageBox(NULL, TEXT("DrawPrimitiveUp Error"), TEXT("DrawPrimitiveUp Error"), MB_OK);
}