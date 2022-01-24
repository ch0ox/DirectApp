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
#define D3DFVF_NVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_NORMAL)	// Test

#define idle 0
#define over 1
#define click 2

typedef struct NVERTEX
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT nx, ny, nz;
}test;

typedef struct COLORVERTEX
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT u, v;				// TEXTURE ��ǥ
}colorVertex;

typedef struct RHWVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
	FLOAT u, v;				// TEXTURE ��ǥ
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
	VOID DrawRect(CButton* pButton);
	VOID LinkD3D(CDxDriver* pDriver);
	VOID TextInit();

	VOID DrawTextFPS();
	VOID DrawTriangle();

	VOID SetDuringTime(FLOAT time);

	std::vector<CButton*>				m_btnVector;
	std::vector<CButton*>::iterator		m_btnIter;

	RHWVERTEX vertices[4];
	COLORVERTEX textureVertices[4];

	// Triangle
	FLOAT m_fAngle = 0.0f;
	FLOAT m_fScale = 1.0f;

	LPDIRECT3DTEXTURE9 m_pTexture;

private:
	CDxDriver* m_pDriver = nullptr;
	FLOAT m_duringTime = 0;

protected:
	UINT iVtxSize = sizeof(COLORVERTEX);										// ���� ���� ����ü ������
	UINT iVtxNum = 3;															// ���� ����
	DWORD dwFVF = D3DFVF_COLORVERTEX;											// ���ؽ� ���� �÷���

	UINT iTriSize = sizeof(INDEX);												// �ﰢ�� �ε��� ����ü ������
	UINT iTriNum = 1;						// �ﰢ�� 1���� �ʱ�ȭ				// �ﰢ�� ����
	D3DFORMAT vFormat = D3DFMT_INDEX32;											//���� ����

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





