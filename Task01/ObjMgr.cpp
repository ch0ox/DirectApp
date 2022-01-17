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
	DWORD umap_index = 1;

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
			// str : f ������ ����� ������ vector<string>
			str = StrtokString((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			int vertices = str.size();

			CFace tmpFace;
			// faceOrder : f ���� ������ ���� ��ȣ
			for (int faceOrder = 0; faceOrder < vertices; faceOrder++)
			{
				vi = StrtokInt((char*)str[faceOrder].c_str(), (char*)"/");
				if (vi.size() == 3)
				{
					p3i.d = { vi[0] - prev_v, vi[1] - prev_vt, vi[2] - prev_vn };
					//tmpFace.v_pairs.push_back(p3i);
				}
				else		// If it doesn't have vt. 'No Texture Vector'
				{
					p3i.d = { vi[0] - prev_v, NULL , vi[1] - prev_vn };
					//tmpFace.v_pairs.push_back(p3i);
				}

				if (!m_uMap.count(str[faceOrder]))								// �ش� face string ( ex. _/_/_ ) �� ���� ?
				{
					m_uMap.insert(make_pair(str[faceOrder], umap_index));		// �ش� face string , index INSERT.
					m_indices.push_back(umap_index);
					umap_index++;

					// Face �� �ش�Ǵ� Vertex ����
					OBJVERTEX tmpVtx = FaceToVertex(objCnt - 1, p3i);
					m_vertices.push_back(tmpVtx);
				}
				else															// �ش� face string ( ex. _/_/_ ) �� ���� ?
				{
					DWORD index = m_uMap[str[faceOrder]];
					m_indices.push_back(index);
				}

			}
			//m_objs[objCnt - 1].f.push_back(tmpFace);

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

OBJVERTEX CObjMgr::FaceToVertex(int num, CPoint3i tmp)
{
	float x, y, z, nx, ny, nz, u, v;
	int v_id, vt_id, vn_id;

	OBJVERTEX tmpVtx;

	v_id = tmp.d[0];
	vt_id = tmp.d[1];
	vn_id = tmp.d[2];


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
	return tmpVtx;
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

//				DWORD index = AddVertex((UINT)v_id, &tmpVtx);			// ���° vtx ����, �ش� tmp �� �߰��� �� �ִ���(���� vtx �� �Ȱ�ġ����) Ȯ��.
																	// �߰��� �� ������ AddVertex �Լ� �ȿ��� vtx �߰�.
																	// v_id �� hash �� ����
																	// 
				// TO DO : ���⼭ � index ���� indices �� ������ dword ��ȯ���� �޴�,
				//			FaceVertex �� v_id , vt_id , vn_id �־�ΰ�
				//			v_id �� ����Ǿ��ִ� hashTable , vt_id �� ����Ǿ��ִ� hashTable, vn_id �� ����Ǿ��ִ� hashTable �� �����ΰ�
				//			v_id HashTable : unordered_map<UINT (hash) , Node* > �� Node�� hash(�ش� v_id)���� ����� vt_id �� Node
				//			vt_id HashTable : unordered_map<UINT (hash) , Node*> �� Node �� hash(�ش� vt_id) ���� ����� vn_id �� Node
				//			vn_id HashTable : unordered_map<UINT (hash) , Node*> �� Node �� hash (�ش� vn_id) ���� ����� ...

//				if (index == (DWORD)-1)								
//				{
//					MessageBox(NULL, TEXT("[ index = AddVertex() ] Function Error"), TEXT("Error"), MB_OK);
//				}
//				index += 1;											// index �� 1���� �����ϱ� ������ +1
//				m_indices.push_back(index);						

			}
		}
		// TO DO : Create Buffer
		// obj �ϳ��� �Ѱܼ� obj �ϳ��� ���� ���� ����.
		// �ش� obj �� ���� vertex ���� (vec[0], vec[1], ... )			-> ���� ��뿡�� �ؽ����̺� ������� �ٲ㼭 CreateObjBuffer ������.
		// ������ vec ���� v,vt,vn ������ �������.

//		vtx.clear();
//		idx.clear();

		//CreateObjBuffer(m_objs[num], pDriver);
	}
	

}



// To Create Obj Buffer. each obj.
VOID CObjMgr::CreateObjBuffer(CObj obj,CDxDriver* pDriver)
{
//	if (!vtxVec.empty())
	{
		m_dwFVF = obj.fvf;

		UINT index = pDriver->CreateObjVertexBuffer(m_vertices.size() * m_vtxSize, 0, m_dwFVF, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		HRESULT hr = pDriver->CopyObjVertexBuffer(index, &m_vertices[0], sizeof(m_vertices));
		if (FAILED(hr))
			return;

		index = pDriver->CreateObjIndexBuffer(m_indices.size() * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		hr = pDriver->CopyObjIndexBuffer(index, &m_indices[0], sizeof(m_indices));


		m_vertices.clear();
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

BOOL CObjMgr::ObjLoad2(std::ifstream&, CObjMgr*)
{
	return TRUE;
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





