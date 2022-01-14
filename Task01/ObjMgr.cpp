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
		// o, v, vt, vn, f �̿��� Line
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
		MessageBox(NULL, elseLine, TEXT("���� ����"), MB_OK);
	}
	

	return TRUE;
}




// Save Obj File Data.
/* �� �Լ����� Buffer ���� ������.*/
VOID CObjMgr::ObjData(CDxDriver* pDriver)
{
	float x, y, z, nx, ny, nz, u, v;
	int v_id, vt_id, vn_id;

	// Ȯ�ο� �ڵ�
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


				// ���� ��ǥ (v)
				tmpVtx.x = m_objs[num].v[v_id - 1].d[0];
				tmpVtx.y = m_objs[num].v[v_id - 1].d[1];
				tmpVtx.z = m_objs[num].v[v_id - 1].d[2];

				// ���� ���� ��ǥ (vn)
				tmpVtx.nx = m_objs[num].vn[vn_id - 1].d[0];
				tmpVtx.ny = m_objs[num].vn[vn_id - 1].d[1];
				tmpVtx.nz = m_objs[num].vn[vn_id - 1].d[2];

				// �ؽ��� ��ǥ (vt)
				if (vt_id != NULL)									// If it has Textures ?
				{
					m_bIsTexturing = TRUE;

					tmpVtx.u = m_objs[num].vt[vt_id - 1].d[0];
					tmpVtx.v = m_objs[num].vt[vt_id - 1].d[1];
					m_objs[num].fvf = D3DFVF_TEXTUREVERTEX;			// Texture ���� ���, FVF �� �ؽ��� ������ ����.
				}
				else
				{
					m_bIsTexturing = FALSE;

					tmpVtx.u = NULL;
					tmpVtx.v = NULL;	
					m_objs[num].fvf = D3DFVF_NOTEXTUREVERTEX;			// Texture ���� ���, FVF �� �÷���(D3DFVF_DIFFUSE)���� ����.
				}

				// TO DO : AddVertex �Լ� �����ؾ��� �ڲ� �̻��� ���� ��

				DWORD index = AddVertex((UINT)j, &tmpVtx);			// ���° vtx ����, �ش� tmp �� �߰��� �� �ִ���(���� vtx �� �Ȱ�ġ����) Ȯ��.
																	// �߰��� �� ������ AddVertex �Լ� �ȿ��� vtx �߰�.

				if (index == (DWORD)-1)								
				{
					MessageBox(NULL, TEXT("[ index = AddVertex() ] Function Error"), TEXT("Error"), MB_OK);
				}
				index += 1;											// index �� 1���� �����ϱ� ������ +1
				m_indices.push_back(index);						
				
			}
		}
		// TO DO : Create Buffer
		// obj �ϳ��� �Ѱܼ� obj �ϳ��� ���� ���� ����.
		// �ش� obj �� ���� vertex ���� (vec[0], vec[1], ... )			-> ���� ��뿡�� �ؽ����̺� ������� �ٲ㼭 CreateObjBuffer ������.
		// ������ vec ���� v,vt,vn ������ �������.
		
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
		// TO DO : ���� - while ���� �ι�°���� �ȵ��� (pNode->pNext �� nullptr�� ����)
		while (pNode != nullptr)
		{
			OBJVERTEX* pSrc = m_vertices.GetData() + pNode->index;

			// Vertices �� �ش� pVtx �� �ִٸ�
			// pSrc == pVtx ������ �߰�!
			// Exist
			if (memcmp(pVtx, pSrc, sizeof(OBJVERTEX)) == 0)					// TO DO : obj ������ �ι� �ҷ����� ��� ���ܹ߻��� -> ������
			{
				bIsExist = TRUE;
				index = pNode->index;	// ���� �ִ� index ã�Ƽ� �ֱ�
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
		// �ش� vtx �߰�
		m_vertices.Add(*pVtx);

		Node* pNewNode = new Node;
		if (pNewNode == nullptr)
		{
			return (DWORD)-1;
		}

		// New node �� index �߰�
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

			// �� ���� ã�Ҵ�~ ����
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

		// TO DO : �� ������ DxDriver ���� �Ұ���
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

	// ����� ������, ����鼭 ������..
// 	pDriver->m_pVertexBufferList.push_back(obj.m_pVB);
// 	pDriver->m_pIndexBufferList.push_back(obj.m_pIB);
}

VOID CObjMgr::ObjDraw(CObjMgr objMgr, CDxDriver* pDriver)
{

/* Draw �κ� */
	if (m_bIsTexturing)
	{
		// TO DO : ��ǥ�� �°� �ؽ��� ������.
		//pDriver->SetTexture(texture...);
	}
	else
	{
		// No Texturing.
		pDriver->SetTexture(NO_TEXTURE);
		// TO DO : ȸ������ Shading.

	}

	// TO DO : Set Matrices

	// TO DO : ��ǥ�� �°� �׷��ֱ�.
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





