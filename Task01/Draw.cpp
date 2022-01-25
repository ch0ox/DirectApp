/*------------------------------------------------------------------------*/
//								CDraw.cpp
/*------------------------------------------------------------------------*/


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

}

CDraw::~CDraw()
{

}

VOID CDraw::SetDuringTime(FLOAT time)
{
	m_duringTime = time;
}

VOID CDraw::TextInit(CDxDriver* pDriver)
{
	if (pDriver->m_pFont != NULL)
	{
		SAFE_RELEASE(pDriver->m_pFont);
		pDriver->m_pFont = NULL;
	}

	D3DXFONT_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Height = 15;										// ±ÛÀÚ ³ôÀÌ
	desc.Width = 12;										// ±ÛÀÚ ³ÐÀÌ
	desc.Weight = FW_NORMAL;								// ±ÛÀÚ ±½±â		// FW_BOLD (±½)
	desc.MipLevels = D3DX_DEFAULT;
	desc.Italic = FALSE;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.Quality = DEFAULT_QUALITY;
	desc.PitchAndFamily = DEFAULT_PITCH;
	desc.FaceName, TEXT("µ¸¿òÃ¼");

	pDriver->m_pDesc = desc;
	D3DXCreateFontIndirect(pDriver->m_pD3DDevice, &(pDriver->m_pDesc), &(pDriver->m_pFont));	// ÆùÆ® »ý¼º

	return;
}


VOID CDraw::DrawTextFPS(CDxDriver* pDriver)
{
	int ScreenWidth = (static_cast<int>(GetSystemMetrics(SM_CXSCREEN)));
	int ScreenHeight = (static_cast<int>(GetSystemMetrics(SM_CYSCREEN)));

	if (pDriver->m_pFont == NULL) return;

	RECT rect;
	TCHAR str[1024];
	if (pDriver->WindowMode)
		SetRect(&rect, 0, 5, nWidth, nHeight);
	else
		SetRect(&rect, 0, 5, ScreenWidth, ScreenHeight);

	ZeroMemory(str, sizeof(char) * MAX_PATH);
	wsprintf(str, TEXT("FPS=%d"), static_cast<int>(m_duringTime));
	pDriver->m_pFont->DrawTextA(NULL, (LPCSTR)str, 15, &rect, DT_TOP | DT_CENTER, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));

	return;
}

// Use D3DFVF_XYZ and DrawIndexedPrimitive.
VOID CDraw::DrawTriangle(CDxDriver* pDriver)
{
	pDriver->m_pD3DDevice->SetStreamSource(0, pDriver->m_pVB_Tri, 0, sizeof(COLORVERTEX));
	pDriver->m_pD3DDevice->SetFVF(D3DFVF_COLORVERTEX);
	// SetIndices
	pDriver->m_pD3DDevice->SetIndices(pDriver->m_pIB_Tri);
	HRESULT hr = pDriver->m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
	if (FAILED(hr))
		MessageBox(NULL, TEXT("DrawIndexedPrimitive Error"), TEXT("DrawIndexedPrimitive Error"), MB_OK);
}

HRESULT CDraw::CreateTriangleBuffer(CDxDriver* pDriver)
{
	// Culling OFF.
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	if (FAILED(pDriver->m_pD3DDevice->CreateVertexBuffer(iVtxNum * iVtxSize, 0, dwFVF, D3DPOOL_MANAGED, &(pDriver->m_pVB_Tri), NULL)))
	{
		MessageBox(NULL, TEXT("Vertex Buffer Error"), TEXT("Vertex Buffer Error"), MB_OK);
		return E_FAIL;
	}

	if (FAILED(pDriver->m_pD3DDevice->CreateIndexBuffer(iTriNum * iTriSize, 0, vFormat, D3DPOOL_MANAGED, &(pDriver->m_pIB_Tri), NULL)))
	{
		MessageBox(NULL, TEXT("Index Buffer Error"), TEXT("Index Buffer Error"), MB_OK);
		return E_FAIL;
	}

	COLORVERTEX triangle[] =
	{
				{ -1.0f, -1.0f, 0.0f, 0xffffffff, 0.0f, 1.0f },
				{ 1.0f, -1.0f, 0.0f, 0xffffffff, 1.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 0xffffffff, 0.5f, 0.0f },
	};

	UINT triangleIndex[3] =
	{
		2, 0, 1,
	};

	// Triangle
	VOID* pVertices;
	pDriver->m_pVB_Tri->Lock(0, sizeof(triangle), &pVertices, 0);
	memcpy(pVertices, triangle, sizeof(triangle));
	pDriver->m_pVB_Tri->Unlock();

	VOID* pIndices;
	pDriver->m_pIB_Tri->Lock(0, sizeof(triangleIndex), &pIndices, 0);
	memcpy(pIndices, triangleIndex, sizeof(triangleIndex));
	pDriver->m_pIB_Tri->Unlock();

	//D3DXCreateTextureFromFile(pDriver->m_pD3DDevice, TEXT("tree2.png"), &m_pTexture);

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