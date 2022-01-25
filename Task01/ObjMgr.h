#pragma once

#ifndef __CHAE_ObjMgr_H
#define __CHAE_ObjMgr_H

#include "Draw.h"
#include <windows.h>
#include <D3dx9tex.h>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>

#define START_CONTEXT 2
#define START_CONTEXT_else 3

//								V				Vt				Vn
#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL )		// | D3DFVF_DIFFUSE 
#define D3DFVF_NOTEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL )		// | D3DFVF_DIFFUSE 
//#define D3DFVF_NOTEXTUREVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)

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
	float x, y, z;				// VERTEX 좌표
	float u, v;					// TEXTURE 좌표
	float nx, ny, nz;			// NORMAL 좌표
	//DWORD color = 0x666666;	// gray
}objVertex;


class CObj
{
public:
	std::string				name;
	std::vector<CPoint3f>	v;
	std::vector<CPoint2f>	vt;
	std::vector<CPoint3f>	vn;
	std::vector<CFace>		f;

	DWORD fvf;
};

class CMtl
{
public:
	std::string				name;			// mtl name
	D3DXVECTOR3				Ns;				// 재질의 반사 지수 (반사광 하이라이트 초점)
	D3DXVECTOR3				Ka;				// 주변 반사율 (ambient)
	D3DXVECTOR3				Kd;				// 기본 재질 색상 (diffuse color)
	D3DXVECTOR3				Ks;				// 정반사율 지정 (specular color)
	D3DXVECTOR3				Ke;				// 표면에서 방출되는 빛의 색상 (emission color)
	FLOAT					Ni;				// 표면의 광하 밀도 (굴절률)
	FLOAT					d;				// 재질의 투명성
	UINT					illum;			// 재료에 사용할 조명 모델
	std::string				map_Kd;			// diffuse Texture map
	std::string				map_Bump;		// bump map (normal map)
	std::string				map_Ks;
};



/*------------------------------------------------------------------------*/
//								CObjMgr.h
/*------------------------------------------------------------------------*/

class CObjMgr
{
	typedef std::vector<OBJVERTEX>			OBJVERTEXLIST;
	typedef std::vector<DWORD>				INDEXLIST;
public:
	CObjMgr();
	virtual ~CObjMgr();

	BOOL ObjLoad(std::ifstream&, CDxDriver* pDriver);
	BOOL ObjMtlLoad();
	OBJVERTEX FaceToVertex(int, CPoint3i);
	VOID SaveToListIndices(INDEXLIST list);

	VOID CreateObjBuffer(CDxDriver* pDriver);
	VOID ObjDraw(CDxDriver* pDriver);
	DWORD GetFVF() { return m_dwFVF; }

	std::vector <CObj> m_objs;								// Objects Vector
	std::vector <FLOAT> StrtokFloat(char*, char*);
	std::vector <std::string> StrtokString(char*, char*);
	std::vector <INT> StrtokInt(char*, char*);
	std::wstring StringToLPCWSTR(const std::string& str);

// 	CArray <Node*>		m_nodes;
// 	CArray <OBJVERTEX>	m_vertices;

	OBJVERTEXLIST							m_vertices;
	INDEXLIST								m_list_indices;
	std::unordered_map<std::string, DWORD>	m_uMap;
	std::vector<OBJVERTEXLIST>				m_verticesList;
	std::vector<INDEXLIST>					m_indicesList;
	std::vector<INDEXLIST>					m_list_indicesList;					// triangle_list
	std::vector<BOOL>						m_bIsTexturingList;


private:
	
	bool m_bIsTexturing = FALSE;
	VOID Term();
	std::string m_mtl_str = "";

protected:
	UINT m_vtxSize = sizeof(OBJVERTEX);											// 정점 정보 구조체 사이즈
	UINT m_vtxNum = 0;															// 정점 갯수
	DWORD m_dwFVF = D3DFVF_NOTEXTUREVERTEX;										// 버텍스 포맷 플래그

	UINT m_indexSize = sizeof(DWORD);											// 인덱스 구조체 사이즈
	UINT m_indexNum = 0;														// 삼각형 갯수
	D3DFORMAT m_vFormat = D3DFMT_INDEX32;										//버퍼 포맷
	D3DPRIMITIVETYPE m_primitiveType = D3DPT_TRIANGLELIST;						//Draw Type

};




#endif //__CHAE_ObjMgr_H