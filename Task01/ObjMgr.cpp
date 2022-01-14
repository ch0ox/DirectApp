// To Parse Object File.
/*------------------------------------------------------------------------*/
//								CObjMgr.cpp
/*------------------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS

#include "ObjMgr.h"
#include "DXDriver.h"
#include "WinApp.h"
#include "Array.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <map>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

CObjMgr::CObjMgr()
{

}

CObjMgr::~CObjMgr()
{
	
}

BOOL CObjMgr::ObjLoad(std::ifstream& file)
{
	if (!m_objs.empty())
	{
		m_objs.clear();
	}

	std::string line = "";
	std::string anotherLine = "";
	CObj tmpObj;
	int objCnt = 0;
	int lineNum = 1;

	// prev vertex count
	int prev_v = 0;
	int prev_vt = 0;
	int prev_vn = 0;

	// One by One Line
	while (std::getline(file, line))
	{
		int len = line.length();
		std::vector<FLOAT> vf;
		std::vector<std::string> str;
		std::vector<INT> vi;
		CPoint3f p3f;
		CPoint2f p2f;
		CPoint3i p3i;

		// o
		if ((line[0] == 'o' && line[1] == ' '))	// || (line[0] == 'g'  && line[1] == ' '))
		{
			tmpObj.name = line.substr(START_CONTEXT, len - START_CONTEXT);
			m_objs.push_back(tmpObj);

			if (objCnt > 0)
			{
				prev_v += m_objs[objCnt - 1].v.size();
				prev_vt += m_objs[objCnt - 1].vt.size();
				prev_vn += m_objs[objCnt - 1].vn.size();
			}
			objCnt += 1;
		}

		// v
		else if (line[0] == 'v' && line[1] == ' ')							// vertex ?
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			p3f.d = { vf[0], vf[1], vf[2] };
			m_objs[objCnt - 1].v.push_back(p3f);
		}

		// vt
		else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')	// vertex texture ?
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			p2f.d = { vf[0], vf[1] };
			m_objs[objCnt - 1].vt.push_back(p2f);
		}

		// vn
		else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')	// vertex normal ?
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			p3f.d = { vf[0], vf[1], vf[2] };
			m_objs[objCnt - 1].vn.push_back(p3f);
		}

		//f
		else if (line[0] == 'f' && line[1] == ' ')						// face ?
		{
			str = StrtokString((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			int vertices = str.size();
			CFace tmpFace;
			for (int i = 0; i < vertices; i++)
			{
				vi = StrtokInt((char*)str[i].c_str(), (char*)"/");
				if (vi.size() == 3)
				{
					p3i.d = { vi[0] - prev_v, vi[1] - prev_vt, vi[2] - prev_vn };
					tmpFace.v_pairs.push_back(p3i);
				}
				else		// If it doesn't have vt. 'No Texture Vector'
				{
					p3i.d = { vi[0] - prev_v, NULL , vi[1] - prev_vn };
					tmpFace.v_pairs.push_back(p3i);
				}
			}
			m_objs[objCnt - 1].f.push_back(tmpFace);
		}
		// o, v, vt, vn, f 이외의 Line
		else
		{
			anotherLine += std::to_string(lineNum);
			anotherLine += " ";
			anotherLine += line;
			anotherLine += "\n";
		}

		lineNum++;
		vf.clear();
		vi.clear();
		str.clear();
	}

	std::wstring tmp = StringToLPCWSTR(anotherLine);
	LPCWSTR elseLine = tmp.c_str();
	if (elseLine != NULL)
	{
		MessageBox(NULL, elseLine, TEXT("예외 라인"), MB_OK);
	}
	

	return TRUE;
}




// Save Obj File Data.
/* 이 함수에서 Buffer 생성 까지만.*/
VOID CObjMgr::ObjData(CDxDriver* pDriver)
{
	float x, y, z, nx, ny, nz, u, v;
	int v_id, vt_id, vn_id;

	// 확인용 코드
	if (NULL != m_objs.size())
	{
		std::string tmp = std::to_string(m_objs.size());
		std::wstring tmp2 = StringToLPCWSTR(tmp);
		LPCWSTR lpcwstr = tmp2.c_str();

		MessageBox(0, lpcwstr, TEXT("objs size"), MB_OK);
	}

	// num : obj number
	for (int num = 0; num < m_objs.size(); num++)
	{
		int f = m_objs[num].f.size();
//		std::vector<OBJVERTEX> vtx;
//		std::vector<DWORD> idx;
		OBJVERTEX tmpVtx;


		for (int i = 0; i < f; i++)
		{
			int points = m_objs[num].f[i].v_pairs.size();
			for (int j = 0; j < points; j++)
			{
				v_id = m_objs[num].f[i].v_pairs[j].d[0];
				vt_id = m_objs[num].f[i].v_pairs[j].d[1];
				vn_id = m_objs[num].f[i].v_pairs[j].d[2];


				// 정점 좌표 (v)
				tmpVtx.x = m_objs[num].v[v_id - 1].d[0];
				tmpVtx.y = m_objs[num].v[v_id - 1].d[1];
				tmpVtx.z = m_objs[num].v[v_id - 1].d[2];

				// 법선 벡터 좌표 (vn)
				tmpVtx.nx = m_objs[num].vn[vn_id - 1].d[0];
				tmpVtx.ny = m_objs[num].vn[vn_id - 1].d[1];
				tmpVtx.nz = m_objs[num].vn[vn_id - 1].d[2];

				// 텍스쳐 좌표 (vt)
				if (vt_id != NULL)									// If it has Textures ?
				{
					m_bIsTexturing = TRUE;

					tmpVtx.u = m_objs[num].vt[vt_id - 1].d[0];
					tmpVtx.v = m_objs[num].vt[vt_id - 1].d[1];
					m_objs[num].fvf = D3DFVF_TEXTUREVERTEX;			// Texture 있을 경우, FVF 를 텍스쳐 용으로 설정.
				}
				else
				{
					m_bIsTexturing = FALSE;

					tmpVtx.u = NULL;
					tmpVtx.v = NULL;	
					m_objs[num].fvf = D3DFVF_NOTEXTUREVERTEX;			// Texture 없을 경우, FVF 를 컬러용(D3DFVF_DIFFUSE)으로 설정.
				}

				// TO DO : AddVertex 함수 수정해야함 자꾸 이상한 값이 들어감

				DWORD index = AddVertex((UINT)j, &tmpVtx);			// 몇번째 vtx 인지, 해당 tmp 를 추가할 수 있는지(이전 vtx 와 안겹치는지) 확인.
																	// 추가할 수 있으면 AddVertex 함수 안에서 vtx 추가.

				if (index == (DWORD)-1)								
				{
					MessageBox(NULL, TEXT("[ index = AddVertex() ] Function Error"), TEXT("Error"), MB_OK);
				}
				index += 1;											// index 는 1부터 시작하기 때문에 +1
				m_indices.push_back(index);						
				
			}
		}
		// TO DO : Create Buffer
		// obj 하나만 넘겨서 obj 하나에 대한 버퍼 생성.
		// 해당 obj 에 대한 vertex 벡터 (vec[0], vec[1], ... )			-> 벡터 사용에서 해쉬테이블 사용으로 바꿔서 CreateObjBuffer 수정함.
		// 각각의 vec 에는 v,vt,vn 정보가 들어있음.
		
//		vtx.clear();
//		idx.clear();

		CreateObjBuffer(m_objs[num], pDriver);
	}
	

}



DWORD CObjMgr::AddVertex(UINT hash, OBJVERTEX* pVtx)
{
	DWORD index = (DWORD)-1;
	BOOL bIsExist = FALSE;

	// TO DO : Is Exist ?
	if ((UINT)m_nodes.GetSize() > hash)
	{
		Node* pNode = m_nodes.Get(hash);
		// Node Loop
		// TO DO : 수정 - while 문에 두번째부터 안들어옴 (pNode->pNext 가 nullptr이 나옴)
		while (pNode != nullptr)
		{
			OBJVERTEX* pSrc = m_vertices.GetData() + pNode->index;

			// Vertices 에 해당 pVtx 가 있다면
			// pSrc == pVtx 같은거 발견!
			// Exist
			if (memcmp(pVtx, pSrc, sizeof(OBJVERTEX)) == 0)					// TO DO : obj 파일을 두번 불러오는 경우 예외발생함 -> 수정ㄱ
			{
				bIsExist = TRUE;
				index = pNode->index;	// 원래 있는 index 찾아서 넣기
				break;
			}
			pNode = pNode->pNext;
		}
	}


	// Not Exist
	if (!bIsExist)
	{
		// Last location index ?
		index = m_vertices.GetSize();
		// 해당 vtx 추가
		m_vertices.Add(*pVtx);

		Node* pNewNode = new Node;
		if (pNewNode == nullptr)
		{
			return (DWORD)-1;
		}

		// New node 에 index 추가
		pNewNode->index = index;
		pNewNode->pNext = nullptr;

		while ((UINT)m_nodes.GetSize() <= hash)
		{
			m_nodes.Add(nullptr);
		}

		Node* pCurNode = m_nodes.Get(hash);

		if (pCurNode == nullptr)
		{
			m_nodes.Set(hash, pNewNode);
		}
		else
		{
			while (pCurNode->pNext != nullptr)
			{
				pCurNode = pCurNode->pNext;
			}

			// 새 공간 찾았다~ 연결
			pCurNode->pNext = pNewNode;
		}
	}

	return index;
}

VOID CObjMgr::DeleteNode()
{
	for (int i = 0; i < m_nodes.GetSize(); i++)
	{
		Node* pNode = m_nodes.Get(i);
		while (pNode != nullptr)
		{
			Node* pNext = pNode->pNext;
			delete pNode;
			pNode = nullptr;

			pNode = pNext;
		}
	}
}

// To Create Obj Buffer. each obj.
VOID CObjMgr::CreateObjBuffer(CObj obj,CDxDriver* pDriver)
{
//	if (!vtxVec.empty())
	{
		m_dwFVF = obj.fvf;

		// TO DO : 이 과정을 DxDriver 에서 할거임
// 		if (FAILED(pDriver->m_pD3DDevice->CreateVertexBuffer(m_vertices.GetSize() * m_vtxSize, 0, m_dwFVF, D3DPOOL_MANAGED, &(obj.m_pVB), nullptr)))
// 		{
// 			MessageBox(NULL, TEXT("Obj Vertex Buffer Error"), TEXT("Error"), MB_OK);
// 		}
// 		VOID* pVertices;
// 		obj.m_pVB->Lock(0, m_vtxNum, &pVertices, 0);
// 		memcpy(pVertices, m_vertices.GetData(), m_vertices.GetSize() * m_vtxSize);
// 		obj.m_pVB->Unlock();

// 		if (FAILED(pDriver->m_pD3DDevice->CreateIndexBuffer(m_indices.size() * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED, &(obj.m_pIB), nullptr)))
// 		{
// 			MessageBox(NULL, TEXT("Obj Index Buffer Error"), TEXT("Error"), MB_OK);
// 		}
// 		VOID* pIndices;
// 		obj.m_pIB->Lock(0, m_indexNum, &pIndices, 0);
// 		memcpy(pIndices, &m_indices[0], m_indices.size() * m_indexSize);
// 		obj.m_pIB->Unlock();


		UINT index = pDriver->CreateObjVertexBuffer(m_vertices.GetSize() * m_vtxSize, 0, m_dwFVF, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		HRESULT hr = pDriver->CopyObjVertexBuffer(index, m_vertices.GetData(), sizeof(m_vertices));
		if (FAILED(hr))
			return;

		index = pDriver->CreateObjIndexBuffer(m_indices.size() * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		hr = pDriver->CopyObjIndexBuffer(index, &m_indices[0], sizeof(m_indices));


		m_vertices.Remove();
		m_indices.clear();
	}
	// SetIndices
	pDriver->m_pD3DDevice->SetIndices(obj.m_pIB);

	// 만들고 넣을지, 만들면서 넣을지..
// 	pDriver->m_pVertexBufferList.push_back(obj.m_pVB);
// 	pDriver->m_pIndexBufferList.push_back(obj.m_pIB);
}

VOID CObjMgr::ObjDraw(CObjMgr objMgr, CDxDriver* pDriver)
{

/* Draw 부분 */
	if (m_bIsTexturing)
	{
		// TO DO : 좌표에 맞게 텍스쳐 입히기.
		//pDriver->SetTexture(texture...);
	}
	else
	{
		// No Texturing.
		pDriver->SetTexture(NO_TEXTURE);
		// TO DO : 회색으로 Shading.

	}

	// TO DO : Set Matrices

	// TO DO : 좌표에 맞게 그려주기.
	Render(objMgr, pDriver);
}

VOID CObjMgr::Render(CObjMgr obj, CDxDriver* pDriver)
{
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDriver->m_pD3DDevice->SetFVF(obj.GetFVF());
	pDriver->m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_world);
}



std::vector <FLOAT> CObjMgr::StrtokFloat(char* str, char* delimeter)
{
	std::vector<FLOAT> vec;
	char* context = { 0, };
	char* tok = strtok_s(str, delimeter, &context);

	while (tok != NULL)
	{
		vec.push_back(atof(tok));		// context to float
		tok = strtok_s(context, delimeter, &context);
	}
	return vec;
}

std::vector <std::string> CObjMgr::StrtokString(char* str, char* delimeter)
{
	std::vector<std::string> vec;
	char* context = { 0, };
	char* tok = strtok_s(str, delimeter, &context);

	while (tok != NULL)
	{
		vec.push_back(tok);		
		tok = strtok_s(context, delimeter, &context);
	}
	return vec;
}

std::vector <INT> CObjMgr::StrtokInt(char* str, char* delimeter)
{
	std::vector<INT> vec;
	char* context = { 0, };
	char* tok = strtok_s(str, delimeter, &context);

	while (tok != NULL)
	{
		vec.push_back(atoi(tok));		// context to int
		tok = strtok_s(context, delimeter, &context);
	}
	return vec;
}


std::wstring CObjMgr::StringToLPCWSTR(const std::string& str)
{
	int length = (int)str.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), length, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), length, buf, len);
	std::wstring r(buf);
	delete[] buf;

	return r;
}



// To Save Object Data.
/*------------------------------------------------------------------------*/
//								CObjModel.cpp
/*------------------------------------------------------------------------*/
CObjModel::CObjModel()
{

}

CObjModel::~CObjModel()
{

}





