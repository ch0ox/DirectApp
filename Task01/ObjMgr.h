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
	float x, y, z;				// VERTEX ��ǥ
	float u, v;					// TEXTURE ��ǥ
	float nx, ny, nz;			// NORMAL ��ǥ
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
	D3DXVECTOR3				Ns;				// ������ �ݻ� ���� (�ݻ籤 ���̶���Ʈ ����)
	D3DXVECTOR3				Ka;				// �ֺ� �ݻ��� (ambient)
	D3DXVECTOR3				Kd;				// �⺻ ���� ���� (diffuse color)
	D3DXVECTOR3				Ks;				// ���ݻ��� ���� (specular color)
	D3DXVECTOR3				Ke;				// ǥ�鿡�� ����Ǵ� ���� ���� (emission color)
	FLOAT					Ni;				// ǥ���� ���� �е� (������)
	FLOAT					d;				// ������ ����
	UINT					illum;			// ��ῡ ����� ���� ��
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
	UINT m_vtxSize = sizeof(OBJVERTEX);											// ���� ���� ����ü ������
	UINT m_vtxNum = 0;															// ���� ����
	DWORD m_dwFVF = D3DFVF_NOTEXTUREVERTEX;										// ���ؽ� ���� �÷���

	UINT m_indexSize = sizeof(DWORD);											// �ε��� ����ü ������
	UINT m_indexNum = 0;														// �ﰢ�� ����
	D3DFORMAT m_vFormat = D3DFMT_INDEX32;										//���� ����
	D3DPRIMITIVETYPE m_primitiveType = D3DPT_TRIANGLELIST;						//Draw Type

};




#endif //__CHAE_ObjMgr_H