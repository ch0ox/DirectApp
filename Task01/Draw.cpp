/*------------------------------------------------------------------------*/
//								CDraw.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include <windows.h>
#include "Draw.h"
#include "MouseMgr.h"
#include "WinApp.h"
#include <map>
#include <string>
#include "Button.h"
#include <algorithm>



CDraw::CDraw()
{
	TriangleInit();
}

CDraw::~CDraw()
{
	btnIter = this->btnVector.begin();
	for (; this->btnVector.end() != btnIter; ++btnIter)
	{
		delete (*btnIter);
	}
	btnVector.clear();
}

VOID CDraw::LinkD3D(CDxDriver* pDriver)
{
	g_pDriver = pDriver;
}

VOID CDraw::SetDuringTime(FLOAT time)
{
	duringTime = time;
}

VOID CDraw::TextInit()
{
	if (g_pDriver->g_pFont != NULL)
	{
		SAFE_RELEASE(g_pDriver->g_pFont);
		g_pDriver->g_pFont = NULL;
	}

	D3DXFONT_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Height = 15;										// ���� ����
	desc.Width = 12;										// ���� ����
	desc.Weight = FW_NORMAL;								// ���� ����		// FW_BOLD (��)
	desc.MipLevels = D3DX_DEFAULT;
	desc.Italic = FALSE;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.Quality = DEFAULT_QUALITY;
	desc.PitchAndFamily = DEFAULT_PITCH;
	desc.FaceName, TEXT("����ü");

	g_pDriver->g_pDesc = desc;
	D3DXCreateFontIndirect(g_pDriver->pd3dDevice, &(g_pDriver->g_pDesc), &(g_pDriver->g_pFont));	// ��Ʈ ����

	return;
}


VOID CDraw::DrawTextFPS()
{
	int ScreenWidth = (static_cast<int>(GetSystemMetrics(SM_CXSCREEN)));
	int ScreenHeight = (static_cast<int>(GetSystemMetrics(SM_CYSCREEN)));

	if (g_pDriver->g_pFont == NULL) return;

	RECT rect;
	TCHAR str[1024];
	if (g_pDriver->WindowMode)
		SetRect(&rect, 0, 5, nWidth, nHeight);
	else
		SetRect(&rect, 0, 5, ScreenWidth, ScreenHeight);

	ZeroMemory(str, sizeof(char) * MAX_PATH);
	wsprintf(str, TEXT("FPS=%d"), static_cast<int>(duringTime));
	g_pDriver->g_pFont->DrawTextA(NULL, (LPCSTR)str, 15, &rect, DT_TOP | DT_CENTER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));

	return;
}


VOID CDraw::DrawTexture()
{
	g_pDriver->pd3dDevice->SetTexture(0, g_pDriver->g_pTexture);
	/*
		g_pDriver->pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pDriver->pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pDriver->pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pDriver->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);*/

	g_pDriver->pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	g_pDriver->pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	g_pDriver->pd3dDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0xff000000);

	return;
}


VOID CDraw::SetupMatrices()
{

	D3DXMatrixRotationY(&m_matWorld, m_fAngle);
	g_pDriver->pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

	D3DXMatrixLookAtLH(&m_matView, &m_eye, &m_at, &m_up);	// Camera ��ȯ ��� ���
	g_pDriver->pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.f);
	g_pDriver->pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

	D3DXMATRIX matTranslation, matScale, matRotation;
	D3DXQUATERNION vQuatenion;

	FLOAT fRotX = D3DXToRadian(m_anglePitch);
	FLOAT fRotY = D3DXToRadian(m_angleYaw);
	FLOAT fRotZ = D3DXToRadian(m_angleRoll);

	//	D3DXMatrixScaling(&matScale, m_fScale, m_fScale, m_fScale);
	D3DXQuaternionRotationYawPitchRoll(&vQuatenion, fRotY, fRotX, fRotZ);
	D3DXMatrixTranslation(&matTranslation, m_posX, m_posY, m_posZ);

	m_matWorld = matTranslation /* matScale */ * matRotation;
}

VOID CDraw::SetupOrthogonal()
{
	D3DXMATRIX matWorld, matView, matProj;
	// �׵���� ��ȯ
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);

}

// Use D3DFVF_XYZ and DrawIndexedPrimitive.
VOID CDraw::DrawTriangle()
{
	g_pDriver->pd3dDevice->SetStreamSource(0, g_pDriver->g_pVB_Tri, 0, sizeof(COLORVERTEX));
	g_pDriver->pd3dDevice->SetFVF(D3DFVF_COLORVERTEX);
	HRESULT hr = g_pDriver->pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
	if (FAILED(hr))
		MessageBox(NULL, TEXT("DrawIndexedPrimitive Error"), TEXT("DrawIndexedPrimitive Error"), MB_OK);
}

// Use D3DFVF_XYZRHW and DrawPrimitiveUp.
VOID CDraw::DrawRect(CButton* pButton)
{
	g_pDriver->pd3dDevice->SetFVF(D3DFVF_BOXVERTEX);
	HRESULT hr = g_pDriver->pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pButton->m_vertex, sizeof(RHWVERTEX));

	if (FAILED(hr))
		MessageBox(NULL, TEXT("DrawPrimitiveUp Error"), TEXT("DrawPrimitiveUp Error"), MB_OK);
}

VOID CDraw::TriangleInit()
{
	m_anglePitch = 0;
	m_angleYaw = 0;
	m_angleRoll = 0;
	m_posX = 0;
	m_posY = 0;
	m_posZ = 0;

	m_eye.x = 0.0f;
	m_eye.y = 0.0f;
	m_eye.z = -5.0f;

	m_at.x = 0.0f;
	m_at.y = 0.0f;
	m_at.z = 0.0f;

	m_up.x = 0.0f;
	m_up.y = 1.0f;
	m_up.z = 0.0f;

	m_fAngle = 0.0f;
}

HRESULT CDraw::CreateTriangleBuffer()
{
	// Culling OFF.
	g_pDriver->pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDriver->pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (FAILED(g_pDriver->pd3dDevice->CreateVertexBuffer(iVtxNum * iVtxSize, 0, dwFVF, D3DPOOL_MANAGED, &(g_pDriver->g_pVB_Tri), NULL)))
	{
		MessageBox(NULL, TEXT("Vertex Buffer Error"), TEXT("Vertex Buffer Error"), MB_OK);
		return E_FAIL;
	}

	if (FAILED(g_pDriver->pd3dDevice->CreateIndexBuffer(iTriNum * iTriSize, 0, vFormat, D3DPOOL_MANAGED, &(g_pDriver->g_pIB), NULL)))
	{
		MessageBox(NULL, TEXT("Index Buffer Error"), TEXT("Index Buffer Error"), MB_OK);
		return E_FAIL;
	}

	COLORVERTEX triangle[] =
	{
				{ -0.5f, -0.5f, 0.0f, 0xffffffff, 0.0f, 1.0f },
				{ 0.5f, -0.5f, 0.0f, 0xffffffff, 1.0f, 1.0f },
				{ 0.0f, 0.5f, 0.0f, 0xffffffff, 0.5f, 0.0f },
	};

	UINT triangleIndex[3] =
	{
		2, 1, 0
	};

	// Triangle
	VOID* pVertices;
	g_pDriver->g_pVB_Tri->Lock(0, sizeof(triangle), &pVertices, 0);
	memcpy(pVertices, triangle, sizeof(triangle));
	g_pDriver->g_pVB_Tri->Unlock();

	VOID* pIndices;
	g_pDriver->g_pIB->Lock(0, sizeof(triangleIndex), &pIndices, 0);
	memcpy(pIndices, triangleIndex, sizeof(triangleIndex));
	g_pDriver->g_pIB->Unlock();

	// SetIndices
	g_pDriver->pd3dDevice->SetIndices(g_pDriver->g_pIB);


	D3DXCreateTextureFromFile(g_pDriver->pd3dDevice, TEXT("tree2.png"), &m_pTexture);

	return S_OK;

}

HRESULT CDraw::CreateRectBuffer()
{
	RHWVERTEX rectangle[] =
	{
		{ -1.0f, -1.0f, 0.0f, 1.0f, 0xffff0000, 0.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f, 1.0f, 0xffff0000, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f, 0xffff0000, 0.0f, 0.0f },
		{-1.0f, 1.0f, 0.0f, 1.0f, 0xffff0000, 0.0f, 0.0f },
	};
	return S_OK;
}



VOID CDraw::RectangleInit(CButton* pButton)
{
	memset(pButton->m_vertex, 0, sizeof(RHWVERTEX) * 4);

	POINT AnchorPoint;

	// ��ư�� ���� ��������

	if (g_pDriver->WindowMode)
	{
		AnchorPoint = { nWidth / 2, nHeight / 2 };
		pButton->m_vertex[0] = { pButton->m_posX, pButton->m_posY, 1.0f, 1.0f, 0xffffffff, 0.0f, 0.0f };
		pButton->m_vertex[1] = { pButton->m_posX + pButton->m_width, pButton->m_posY, 1.0f, 1.0f, 0xffffffff, 1.0f, 0.0f };
		pButton->m_vertex[2] = { pButton->m_posX, pButton->m_posY + pButton->m_height, 1.0f, 1.0f, 0xffffffff, 0.0f, 1.0f };
		pButton->m_vertex[3] = { pButton->m_posX + pButton->m_width, pButton->m_posY + pButton->m_height, 1.0f, 1.0f, 0xffffffff, 1.0f, 1.0f };

		pButton->SetScale(1.0f, 1.0f);
	}
	else
	{	// ���� �ʿ�
		AnchorPoint = { static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2), static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 2) };
		float ratioX = static_cast<float>(GetSystemMetrics(SM_CXSCREEN) / nWidth);
		float ratioY = static_cast<float>(GetSystemMetrics(SM_CYSCREEN) / nHeight);

		pButton->m_vertex[0] = { (pButton->m_posX) * ratioX, (pButton->m_posY) * ratioY, 1.0f, 1.0f, 0xffffffff, 0.0f, 0.0f };
		pButton->m_vertex[1] = { (pButton->m_posX + pButton->m_width) * ratioX, (pButton->m_posY) * ratioY, 1.0f, 1.0f, 0xffffffff, 1.0f, 0.0f };
		pButton->m_vertex[2] = { (pButton->m_posX) * ratioX, (pButton->m_posY + pButton->m_height) * ratioY, 1.0f, 1.0f, 0xffffffff, 0.0f, 1.0f };
		pButton->m_vertex[3] = { (pButton->m_posX + pButton->m_width) * ratioX, (pButton->m_posY + pButton->m_height) * ratioY, 1.0f, 1.0f, 0xffffffff, 1.0f, 1.0f };

		pButton->SetScale(ratioX, ratioY);
	}
}



VOID CDraw::CreateButton()
{
	// ��ư �ν��Ͻ� 3��
	CButton* pCloseBtn = new CButton("normal.png", "over.png", "click.png", "CloseButton");
	CButton* pLeftBtn = new CButton("left_normal.png", "left_over.png", "left_click.png", "LeftButton");
	CButton* pRightBtn = new CButton("right_normal.png", "right_over.png", "right_click.png", "RightButton");

	btnVector.push_back(pCloseBtn);
	btnVector.push_back(pLeftBtn);
	btnVector.push_back(pRightBtn);

	btnIter = this->btnVector.begin();
	for (; this->btnVector.end() != btnIter; ++btnIter)
	{
		RectangleInit(*btnIter);
	}
}

