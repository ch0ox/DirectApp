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
			// str : f 한줄을 띄어쓰기로 나눠둔 vector<string>
			str = StrtokString((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			int vertices = str.size();

			CFace tmpFace;
			// faceOrder : f 한줄 내에서 순서 번호
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

				if (!m_uMap.count(str[faceOrder]))								// 해당 face string ( ex. _/_/_ ) 이 없음 ?
				{
					m_uMap.insert(make_pair(str[faceOrder], umap_index));		// 해당 face string , index INSERT.
					m_indices.push_back(umap_index);
					umap_index++;

					// Face 에 해당되는 Vertex 정보
					OBJVERTEX tmpVtx = FaceToVertex(objCnt - 1, p3i);
					m_vertices.push_back(tmpVtx);
				}
				else															// 해당 face string ( ex. _/_/_ ) 이 있음 ?
				{
					DWORD index = m_uMap[str[faceOrder]];
					m_indices.push_back(index);
				}

			}
			//m_objs[objCnt - 1].f.push_back(tmpFace);

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

OBJVERTEX CObjMgr::FaceToVertex(int num, CPoint3i tmp)
{
	float x, y, z, nx, ny, nz, u, v;
	int v_id, vt_id, vn_id;

	OBJVERTEX tmpVtx;

	v_id = tmp.d[0];
	vt_id = tmp.d[1];
	vn_id = tmp.d[2];


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
	return tmpVtx;
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

//				DWORD index = AddVertex((UINT)v_id, &tmpVtx);			// 몇번째 vtx 인지, 해당 tmp 를 추가할 수 있는지(이전 vtx 와 안겹치는지) 확인.
																	// 추가할 수 있으면 AddVertex 함수 안에서 vtx 추가.
																	// v_id 를 hash 로 대입
																	// 
				// TO DO : 여기서 어떤 index 값을 indices 에 넣을지 dword 반환값을 받는,
				//			FaceVertex 에 v_id , vt_id , vn_id 넣어두고
				//			v_id 가 저장되어있는 hashTable , vt_id 가 저장되어있는 hashTable, vn_id 가 저장되어있는 hashTable 을 만들어두고
				//			v_id HashTable : unordered_map<UINT (hash) , Node* > 로 Node는 hash(해당 v_id)값에 연결된 vt_id 의 Node
				//			vt_id HashTable : unordered_map<UINT (hash) , Node*> 로 Node 는 hash(해당 vt_id) 값이 연결된 vn_id 의 Node
				//			vn_id HashTable : unordered_map<UINT (hash) , Node*> 로 Node 는 hash (해당 vn_id) 값이 연결된 ...

//				if (index == (DWORD)-1)								
//				{
//					MessageBox(NULL, TEXT("[ index = AddVertex() ] Function Error"), TEXT("Error"), MB_OK);
//				}
//				index += 1;											// index 는 1부터 시작하기 때문에 +1
//				m_indices.push_back(index);						

			}
		}
		// TO DO : Create Buffer
		// obj 하나만 넘겨서 obj 하나에 대한 버퍼 생성.
		// 해당 obj 에 대한 vertex 벡터 (vec[0], vec[1], ... )			-> 벡터 사용에서 해쉬테이블 사용으로 바꿔서 CreateObjBuffer 수정함.
		// 각각의 vec 에는 v,vt,vn 정보가 들어있음.

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





