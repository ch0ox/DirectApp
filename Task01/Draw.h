#pragma once
/*------------------------------------------------------------------------*/
//								CDraw.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_Draw_H
#define __CHAE_Draw_H


#include <D3dx9tex.h>
#include <vector>

class CDxDriver;
class CMouse;
class CButton;

#define D3DFVF_BOXVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_COLORVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define idle 1
#define over 2
#define click 3

typedef struct COLORVERTEX
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT u, v;				// TEXTURE 좌표
}colorVertex;

typedef struct RHWVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
	FLOAT u, v;				// TEXTURE 좌표
}rhwVertex;


typedef struct INDEX
{
	DWORD _0, _1, _2;
}index;


class CDraw
{
public:
	CDraw();
	virtual ~CDraw();

	HRESULT CreateTriangleBuffer();
	HRESULT CreateRectBuffer();
	VOID CreateButton();
	VOID RectangleInit(CButton* pButton);
	VOID DrawRect(CButton* pButton);
	VOID LinkD3D(CDxDriver* pDriver);
	VOID TextInit();
	VOID TriangleInit();

	VOID DrawTextFPS();
	VOID DrawTexture();
	VOID DrawTriangle();

	VOID SetDuringTime(FLOAT time);
	VOID SetupMatrices();
	VOID SetupOrthogonal();

	std::vector<CButton*>				m_btnVector;
	std::vector<CButton*>::iterator		m_btnIter;

	RHWVERTEX vertices[4];
	COLORVERTEX textureVertices[4];

	RHWVERTEX rectangle[4];					// 사각형. 정점 4개
	COLORVERTEX triangle[3];				// 삼각형. 정점 3개

	// Triangle
	FLOAT m_fAngle = 0.0f;
	FLOAT m_fScale = 1.0f;
	D3DXMATRIXA16 m_matWorld;
	D3DXMATRIXA16 m_matView;
	D3DXMATRIXA16 m_matProj;
	D3DXVECTOR3 m_eye, m_at, m_up;
	FLOAT m_anglePitch, m_angleYaw, m_angleRoll;		// X축, Y축, Z축
	FLOAT m_posX, m_posY, m_posZ;
	LPDIRECT3DTEXTURE9 m_pTexture;

private:
	CDxDriver* m_pDriver = nullptr;
	FLOAT m_duringTime = 0;

protected:
	UINT iVtxSize = sizeof(COLORVERTEX);										// 정점 정보 구조체 사이즈
	UINT iVtxNum = 3;															// 정점 갯수
	DWORD dwFVF = D3DFVF_COLORVERTEX;											// 버텍스 포맷 플래그

	UINT iTriSize = sizeof(INDEX);												// 삼각형 인덱스 구조체 사이즈
	UINT iTriNum = 1;						// 삼각형 1개로 초기화				// 삼각형 갯수
	D3DFORMAT vFormat = D3DFMT_INDEX32;											//버퍼 포맷

	D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;						//Draw Type

};





class Delete
{
public:
	template<typename T>
	void operator()(const T* ptr) const
	{
		delete ptr;
		ptr = nullptr;
	}
};



#endif //__CHAE_Draw_H





