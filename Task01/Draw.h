#pragma once
/*------------------------------------------------------------------------*/
//								CDraw.h
/*------------------------------------------------------------------------*/

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

	RHWVERTEX vertices[4];
	COLORVERTEX textureVertices[4];

	RHWVERTEX rectangle[4];				// �簢��. ���� 4��
	COLORVERTEX triangle[3];				// �ﰢ��. ���� 3��

private:
	CDxDriver* g_pDriver = nullptr;

public:
	HRESULT CreateTriangleBuffer();
	HRESULT CreateRectBuffer();
	VOID CreateButton();
	VOID RectangleInit(CButton* pButton);
	VOID DrawRect(CButton* pButton);


public:

	std::vector<CButton*>				btnVector;
	std::vector<CButton*>::iterator		btnIter;

public:
	VOID LinkD3D(CDxDriver* pDriver);
	VOID TextInit();
	VOID TriangleInit();

	VOID DrawTextFPS();
	VOID DrawTexture();
	VOID DrawTriangle();

	VOID SetDuringTime(FLOAT time);
	VOID SetupMatrices();
	VOID SetupOrthogonal();

private:
	FLOAT duringTime = 0;


protected:
	UINT iVtxSize = sizeof(COLORVERTEX);										// ���� ���� ����ü ������
	UINT iVtxNum = 3;															// ���� ����
	DWORD dwFVF = D3DFVF_COLORVERTEX;											// ���ؽ� ���� �÷���

	UINT iTriSize = sizeof(INDEX);												// �ﰢ�� �ε��� ����ü ������
	UINT iTriNum = 1;						// �ﰢ�� 1���� �ʱ�ȭ				// �ﰢ�� ����
	D3DFORMAT vFormat = D3DFMT_INDEX32;											//���� ����

	D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;						//Draw Type

	// Triangle
public:
	FLOAT m_fAngle = 0.0f;
	FLOAT m_fScale = 1.0f;
	D3DXMATRIXA16 m_matWorld;
	D3DXMATRIXA16 m_matView;
	D3DXMATRIXA16 m_matProj;
	D3DXVECTOR3 m_eye, m_at, m_up;
	FLOAT m_anglePitch, m_angleYaw, m_angleRoll;		// X��, Y��, Z��
	FLOAT m_posX, m_posY, m_posZ;
	LPDIRECT3DTEXTURE9 m_pTexture;
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









