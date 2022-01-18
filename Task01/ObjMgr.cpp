// To Parse Object File.
/*------------------------------------------------------------------------*/
//								CObjMgr.cpp
/*------------------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS

#include "ObjMgr.h"
#include "DXDriver.h"
#include "WinApp.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

CObjMgr::CObjMgr()
{

}

CObjMgr::~CObjMgr()
{
	
}

BOOL CObjMgr::ObjLoad(std::ifstream& file, CDxDriver* pDriver)
{
	if (!m_objs.empty())
	{
		m_objs.clear();
	}

	std::string line = "";
	std::string anotherLine = "";
	CObj tmpObj;
	int objCnt = 0;
	int cur_objCnt = 1;
	int lineNum = 1;
	DWORD umap_index = 0;

	// prev vertex count
	int prev_v = 0;
	int prev_vt = 0;
	int prev_vn = 0;

	bool b_face = FALSE;

	OBJVERTEXLIST							vertices;
	INDEXLIST								strip_indices;				// triangle strip
	INDEXLIST								list_indices;				// trinagle list

	std::unordered_map<std::string, DWORD>	uMap;

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
		
		b_face = FALSE;
		


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
			b_face = TRUE;
			// str : f 한줄을 띄어쓰기로 나눠둔 vector<string>
			str = StrtokString((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			int verticesCnt = str.size();
			m_primitiveCount += (verticesCnt - 2) ;		// f가 몇개인지 체크 (polygon 개수 = vertex 개수 - 2 )

			CFace tmpFace;
			// faceOrder : f 한줄 내에서 순서 번호
			for (int faceOrder = 0; faceOrder < verticesCnt; faceOrder++)
			{
				vi = StrtokInt((char*)str[faceOrder].c_str(), (char*)"/");
				if (vi.size() == 3)
				{
					p3i.d = { vi[0] - prev_v, vi[1] - prev_vt, vi[2] - prev_vn };
				}
				else		// If it doesn't have vt. 'No Texture Vector'
				{
					p3i.d = { vi[0] - prev_v, NULL , vi[1] - prev_vn };
				}

				if (!uMap.count(str[faceOrder]))								// 해당 face string ( ex. _/_/_ ) 이 없음 ?
				{
					uMap.insert(make_pair(str[faceOrder], umap_index));		// 해당 face string , index INSERT.
					strip_indices.push_back(umap_index);
					list_indices.push_back(umap_index);
					umap_index++;

					// Face 에 해당되는 Vertex 정보
					OBJVERTEX tmpVtx = FaceToVertex(objCnt - 1, p3i);
					vertices.push_back(tmpVtx);
				}
				else															// 해당 face string ( ex. _/_/_ ) 이 있음 ?
				{
					DWORD index = uMap[str[faceOrder]];
					strip_indices.push_back(index);
					list_indices.push_back(index);
				}

			}
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

		// Face 한줄 읽고나서 traingle_list Add
		if (b_face && (!list_indices.empty()))
		{
			// Make (triangle_list) Indices List.
			SaveToListIndices(list_indices);

			list_indices.clear();
		}

		// obj 가 2개 이상일 경우에만 if 문에 들어감.
		if (cur_objCnt < objCnt)
		{
			// obj 바뀔 때 마다 List 에 Vertex (리스트), Index (리스트) 추가
			m_verticesList.push_back(vertices);
			m_indicesList.push_back(strip_indices);
			m_list_indicesList.push_back(m_list_indices);
			m_primCountList.push_back(m_primitiveCount);
			m_primitiveCount = 0;
			vertices.clear();
			strip_indices.clear();
			uMap.clear();
			if (m_bIsTexturing)
				m_bIsTexturingList.push_back(TRUE);
			else
				m_bIsTexturingList.push_back(FALSE);

			cur_objCnt = objCnt;
		}
	}

	//  file 마지막이 f로 끝났을 때
	if (b_face)
	{
		m_verticesList.push_back(vertices);
		m_indicesList.push_back(strip_indices);
		m_list_indicesList.push_back(m_list_indices);
		m_primCountList.push_back(m_primitiveCount);
		vertices.clear();
		strip_indices.clear();
		uMap.clear();
		if (m_bIsTexturing)
			m_bIsTexturingList.push_back(TRUE);
		else
			m_bIsTexturingList.push_back(FALSE);
	}

	std::wstring tmp = StringToLPCWSTR(anotherLine);
	LPCWSTR elseLine = tmp.c_str();
	if (elseLine != NULL)
	{
		MessageBox(NULL, elseLine, TEXT("예외 라인"), MB_OK);
	}
	
	CreateObjBuffer(pDriver);

	return TRUE;
}

// Save Obj File Data.
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

VOID CObjMgr::SaveToListIndices(INDEXLIST list)
{
//	CCW
//	-------------------------
//  1     3     5     7     9
// 	|   / |   / |   / |   / | 
//	|  /  |  /  |  /  |  /  |
//	| /   | /   | /   | /   |
//  |/    |/    |/    |/    |
//  2     4     6     8     10
//  -------------------------
// STRIP : { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
// LIST  : { {1,2,3}, {3,2,4}, {3,4,5}, {5,4,6}, {5,6,7}, {7,6,8}, {7,8,9}, {9,8,10} }
	
// TO DO : 받아온 list -> list_indices 에 triangle_list 에 맞게 조합.

	INDEXLIST::iterator indiceIter;
	INDEXLIST::iterator tmpIter;
	INDEXLIST::iterator checkIter;


	// triangle list
	INDEXLIST list_indices;				
	// triangle strip iterator
	indiceIter = list.begin();
	tmpIter = indiceIter;

	for (; list.end() != indiceIter; indiceIter++)
	{
		indiceIter = tmpIter;

		list_indices.push_back(*indiceIter);
		list_indices.push_back(*(++indiceIter));
		list_indices.push_back(*(++indiceIter));

		tmpIter = indiceIter;
		checkIter = indiceIter;
		
		if (++checkIter != list.end())
		{
			list_indices.push_back(*indiceIter);
			list_indices.push_back(*(--indiceIter));
			++indiceIter;
			list_indices.push_back(*(++indiceIter));
		}
		else
			break;
	}

	// list_indices -> m_list_indices 에 insert.
	m_list_indices.insert( m_list_indices.end(), list_indices.begin(), list_indices.end() );
	
	list_indices.clear();
}

// To Create Obj Buffer. each obj.
VOID CObjMgr::CreateObjBuffer(CDxDriver* pDriver)
{
	// Culling CCW (반시계)
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		// 안나오면 D3DCULL_NONE 로 확인
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//	if (!vtxVec.empty())
	{
		for (int i = 0; i < m_verticesList.size(); i++)
		{
			DWORD dwFVF;

			if (m_bIsTexturingList[i])
				dwFVF = D3DFVF_TEXTUREVERTEX;
			else
				dwFVF = D3DFVF_NOTEXTUREVERTEX;


			UINT index = pDriver->CreateObjVertexBuffer(m_verticesList[i].size() * m_vtxSize, 0, dwFVF, D3DPOOL_MANAGED);
			if (index == (UINT)-1)
				return;

			HRESULT hr = pDriver->CopyObjVertexBuffer(index, &m_verticesList[i].begin(), sizeof(m_verticesList[i]));
			if (FAILED(hr))
				return;

			// List
			index = pDriver->CreateObjIndexBuffer(m_list_indicesList[i].size() * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
			if (index == (UINT)-1)
				return;

			hr = pDriver->CopyObjIndexBuffer(index, &m_list_indicesList[i].begin(), sizeof(m_list_indicesList[i]));
			if (FAILED(hr))
				return;

			// Strip
// 			index = pDriver->CreateObjIndexBuffer(m_indicesList[i].size() * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
// 			if (index == (UINT)-1)
// 				return;
// 
// 			hr = pDriver->CopyObjIndexBuffer(index, &m_indicesList[i].begin(), sizeof(m_indicesList[i]));
// 			if (FAILED(hr))
// 				return;
		}
	}
}


/* Draw 부분 */
VOID CObjMgr::ObjDraw(CDxDriver* pDriver)
{
	DWORD dwFVF;
	m_eye.x = 0.0f;
	m_eye.y = 0.0f;
	m_eye.z = -5.0f;

	m_at.x = 0.0f;
	m_at.y = 0.0f;
	m_at.z = 0.0f;

	m_up.x = 0.0f;
	m_up.y = 1.0f;
	m_up.z = 0.0f;
//TO DO : Set Matrices
	pDriver->SetWorldMatrix(m_matWorld);
	pDriver->SetCameraMatrix(m_matView, m_eye, m_at, m_up);
	pDriver->SetProjMatrix(m_matProj);

// Drawing

	//pDriver->DrawObjStripModel(this);
	pDriver->DrawObjListModel(this);


// TO DO : Set Texture


	if (m_bIsTexturing)
	{
		// TO DO : 좌표에 맞게 텍스쳐 입히기.
		//pDriver->SetTexture(texture...);
	}
	else
	{
		// No Texturing.
//		pDriver->SetTexture(NO_TEXTURE);
		// TO DO : 회색으로 Shading.

	}
}

const D3DXMATRIX& CObjMgr::GetMatWorld() const
{
	return m_matWorld;
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





