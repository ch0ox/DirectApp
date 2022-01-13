#pragma once

#ifndef __CHAE_ObjMgr_H
#define __CHAE_ObjMgr_H

#include "Draw.h"
#include "Array.h"
#include <windows.h>
#include <D3dx9tex.h>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#define START_CONTEXT 2
#define START_CONTEXT_else 3

#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_DIFFUSE )
#define D3DFVF_NOTEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

#define NO_TEXTURE 0

class CDxDriver;


class CPoint2f
{
public:
	std::vector <FLOAT> d{ 0,0 };
};

class CPoint3f
{
public:
	std::vector <FLOAT> d{ 0,0,0 };
};

class CPoint3i
{
public:
	std::vector <INT> d{ 0,0,0 };
};

class CFace
{
public:
	std::vector<CPoint3i> v_pairs;
};

// HashTable
typedef struct Node
{
	UINT index;
	Node* pNext;
}node;

typedef struct OBJVERTEX
{
	float x, y, z;
	float u, v;				// TEXTURE 좌표
	float nx, ny, nz;
	DWORD color = 0x666666;	// gray
}objVertex;

typedef struct FACEVERTEX
{
	DWORD v, vt, vn;
}faceVertex;


class CObj
{
public:
	std::string				name;
	std::vector<CPoint3f>	v;
	std::vector<CPoint2f>	vt;
	std::vector<CPoint3f>	vn;
	std::vector<CFace>		f;

	std::map<std::string, std::vector<D3DXVECTOR2>> m_tex;

	DWORD fvf;

	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;							// 정점 버퍼
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;							// 인덱스 버퍼
	LPDIRECT3DTEXTURE9			m_pTexture = nullptr;						// 사용할 텍스쳐

};






/*------------------------------------------------------------------------*/
//								CObjMgr.h
/*------------------------------------------------------------------------*/

class CObjMgr
{
public:
	CObjMgr();
	virtual ~CObjMgr();

	BOOL ObjLoad(std::ifstream&);
	VOID ObjData(std::vector<CObj> objs, class CDxDriver* pDriver);
	VOID CreateObjBuffer(CObj obj,CDxDriver* pDriver);
	VOID ObjDraw(CObjMgr obj, CDxDriver* pDriver);
	VOID Render(CObjMgr obj, CDxDriver* pDriver);

	std::vector <CObj> objs;								// Objects Vector
	std::vector <FLOAT> StrtokFloat(char*, char*);
	std::vector <std::string> StrtokString(char*, char*);
	std::vector <INT> StrtokInt(char*, char*);
	std::wstring StringToLPCWSTR(const std::string& str);

private:
	DWORD AddVertex(UINT, OBJVERTEX* pVtx);

	CArray <Node*>		m_nodes;
	CArray <OBJVERTEX>	m_vertices;
//	CArray <DWORD>		m_indices;
	std::vector<DWORD>	m_indices;

	D3DXMATRIXA16 m_world;
	bool m_bIsTexturing = FALSE;


protected:
	UINT m_vtxSize = sizeof(OBJVERTEX);											// 정점 정보 구조체 사이즈
	UINT m_vtxNum = 0;															// 정점 갯수
	DWORD m_dwFVF = D3DFVF_NOTEXTUREVERTEX;										// 버텍스 포맷 플래그

	UINT m_indexSize = sizeof(DWORD);											// 인덱스 구조체 사이즈
	UINT m_indexNum = 0;														// 삼각형 갯수
	D3DFORMAT m_vFormat = D3DFMT_INDEX32;										//버퍼 포맷

	D3DPRIMITIVETYPE m_primitiveType = D3DPT_TRIANGLELIST;						//Draw Type

};




/*------------------------------------------------------------------------*/
//								CObjModel.h
/*------------------------------------------------------------------------*/

class CObjModel
{
public:
	CObjModel();
	virtual ~CObjModel();

private:


};








#endif //__CHAE_ObjMgr_H