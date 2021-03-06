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
	Term();
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
	DWORD origin_index = 0;

	// prev vertex count
	int prev_v = 0;
	int prev_vt = 0;
	int prev_vn = 0;

	int vtCnt = 0;

	bool b_face = FALSE;

	OBJVERTEXLIST							vertices;
	INDEXLIST								strip_indices;				// triangle strip
	INDEXLIST								list_indices;				// trinagle list
	OBJVERTEXLIST							origin_vertices;
	INDEXLIST								origin_Indices;

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
//			p2f.d = { 0, 0 };
			p2f.d = { vf[0], 1 - vf[1] };
			m_objs[objCnt - 1].vt.push_back(p2f);
			/************************************************/
//			std::cout << "u: " << vf[0] << ", v: " << vf[1] << std::endl;
//			std::cout << "u: " << vf[0] << ", 1 - v: " << 1 - vf[1] << std::endl;
 			vtCnt++;
			/************************************************/
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
			// str : f ?????? ?????????? ?????? vector<string>
			str = StrtokString((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			int verticesCnt = str.size();

			CFace tmpFace;
			// faceOrder : f ???? ?????? ???? ????
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

				if (!uMap.count(str[faceOrder]))								// ???? face string ( ex. _/_/_ ) ?? ???? ?
				{
					uMap.insert(make_pair(str[faceOrder], umap_index));		// ???? face string , index INSERT.
					strip_indices.push_back(umap_index);
					list_indices.push_back(umap_index);
					umap_index++;

					// Face ?? ???????? Vertex ????
					OBJVERTEX tmpVtx = FaceToVertex(objCnt - 1, p3i);
					vertices.push_back(tmpVtx);
				}
				else															// ???? face string ( ex. _/_/_ ) ?? ???? ?
				{
					DWORD index = uMap[str[faceOrder]];
					strip_indices.push_back(index);
					list_indices.push_back(index);
				}


				OBJVERTEX tmpV = FaceToVertex(objCnt - 1, p3i);
				origin_vertices.push_back(tmpV);
				origin_Indices.push_back(origin_index);
				origin_index++;

			}
		}

		// mtllib
		else if (line[0] == 'm' && line[1] == 't' && line[2] == 'l' && line[3] == 'l' && line[4] == 'i' && line[5] == 'b' && line[6] == ' ')
		{
			m_mtl_str = line.substr(7, len - 7);
		}


		// o, v, vt, vn, f ?????? Line
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

		// Face ???? ???????? traingle_list Add
		if (b_face && (!list_indices.empty()))
		{
			// Make (triangle_list) Indices List.
			SaveToListIndices(list_indices);
			list_indices.clear();
		}

		if (b_face && (!origin_Indices.empty()))
		{
			SaveToOriginalIndices(origin_Indices);
			origin_Indices.clear();
		}

		// obj ?? 2?? ?????? ???????? if ???? ??????.
		if (cur_objCnt < objCnt)
		{
			// obj ???? ?? ???? List ?? Vertex (??????), Index (??????) ????
			m_verticesList.push_back(vertices);
			m_indicesList.push_back(strip_indices);
			m_list_indicesList.push_back(m_list_indices);
			m_original_V_List.push_back(origin_vertices);
			m_original_I_List.push_back(m_original_indices);
			vertices.clear();
			strip_indices.clear();
			//list_indices.clear();
			//m_list_indices.clear();
			origin_vertices.clear();
			uMap.clear();
			if (m_bIsTexturing)
				m_bIsTexturingList.push_back(TRUE);
			else
				m_bIsTexturingList.push_back(FALSE);

			cur_objCnt = objCnt;
		}
	}

	//  file ???????? f?? ?????? ??
	if (b_face)
	{
		m_verticesList.push_back(vertices);
		m_indicesList.push_back(strip_indices);
		m_list_indicesList.push_back(m_list_indices);
		m_original_V_List.push_back(origin_vertices);
		m_original_I_List.push_back(m_original_indices);
		vertices.clear();
		strip_indices.clear();
		//list_indices.clear();
		//m_list_indices.clear();
		origin_vertices.clear();
		uMap.clear();
		if (m_bIsTexturing)
			m_bIsTexturingList.push_back(TRUE);
		else
			m_bIsTexturingList.push_back(FALSE);
	}

	std::wstring tmp = StringToWstring(anotherLine);
	LPCWSTR elseLine = tmp.c_str();
	if (elseLine != NULL)
	{
		MessageBox(NULL, elseLine, TEXT("???? ????"), MB_OK);
	}

	// is there material file ?
	if (!m_mtl_str.empty())
	{
		std::cout << "material file Exist." << std::endl;
		ObjMtlLoad();
	}
	else
	{
		std::cout << "No material file." << std::endl;
	}

	//CreateObjBuffer(pDriver);
	CreateOriginalObjBuffer(pDriver);
	if (LoadTexture(pDriver))	// Success Load Texture 
	{
		SetObjTex(TRUE);
		std::cout << "Obj Texture file Load Success." << std::endl;
	}
	std::cout << "vt ???? : " << vtCnt << std::endl;
	return TRUE;
}

// Read Obj Material File and Load
BOOL CObjMgr::ObjMtlLoad()
{
	std::ifstream file(m_mtl_str);
	std::string line = "";
	//std::string originPath = "..\\Resource\\";
	std::string originPath = "";
	std::vector<FLOAT> vf;
	CMtl tmpMtl;
	int mtlCnt = 0;

	if (file.fail())
	{
//		MessageBox(NULL, TEXT("Mtl File Load Failed. Can't find the file in folder."), TEXT("Error"), MB_OK);
		std::cout << "Material File Load Failed. Can't find the file in folder." << std::endl;
		return false;
	}

	while (std::getline(file, line))
	{
		int len = line.length();

		if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w' && line[3] == 'm' && line[4] == 't' && line[5] == 'l' && line[6] == ' ')
		{
			++mtlCnt;
			tmpMtl.name = line.substr(7, len - 7);
			m_mtls.push_back(tmpMtl);
		}

		// Ns
		else if (line[0] == 'N' && line[1] == 's' && line[2] == ' ')
		{
			m_mtls[mtlCnt - 1].Ns = std::stof(line.substr(START_CONTEXT_else, len - START_CONTEXT_else));
		}
		
		// Ka
		else if (line[0] == 'K' && line[1] == 'a' && line[2] == ' ')
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			m_mtls[mtlCnt - 1].Ka = { vf[0], vf[1], vf[2] };
		}

		// Kd
		else if (line[0] == 'K' && line[1] == 'd' && line[2] == ' ')
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			m_mtls[mtlCnt - 1].Kd = { vf[0], vf[1], vf[2] };
		}

		// Ks
		else if (line[0] == 'K' && line[1] == 's' && line[2] == ' ')
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			m_mtls[mtlCnt - 1].Ks = { vf[0], vf[1], vf[2] };
		}

		// Ke
		else if (line[0] == 'K' && line[1] == 'e' && line[2] == ' ')
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			m_mtls[mtlCnt - 1].Ke = { vf[0], vf[1], vf[2] };
		}

		// Ni
		else if (line[0] == 'N' && line[1] == 'i' && line[2] == ' ')
		{
			m_mtls[mtlCnt - 1].Ni = std::stof(line.substr(START_CONTEXT_else, len - START_CONTEXT_else));
		}

		// d
		else if (line[0] == 'd' && line[1] == ' ')
		{
			m_mtls[mtlCnt - 1].d = std::stof(line.substr(START_CONTEXT, len - START_CONTEXT));
		}

		// illum
		else if (line[0] == 'i' && line[1] == 'l' && line[2] == 'l' && line[3] == 'u' && line[4] == 'm' && line[5] == ' ')
		{
			m_mtls[mtlCnt - 1].illum = std::stoi(line.substr(6, len - 6));
		}

		// map_ ???? ???????? ????
		else if (line[0] == 'm' && line[1] == 'a' && line[2] == 'p' && line[3] == '_')
		{
			// map_Kd : diffuse
			if (line[4] == 'K' && line[5] == 'd' && line[6] == ' ')
			{
				std::string path = originPath + line.substr(7, len - 7);

				m_mtls[mtlCnt - 1].map_Kd = path;
			}

			// map_Bump : implementation
			else if (line[4] == 'B' && line[5] == 'u' && line[6] == 'm' && line[7] == 'p' && line[8] == ' ')
			{
				std::string path = originPath + line.substr(9, len - 9);
				m_mtls[mtlCnt - 1].map_Bump = path;
			}

			// map_Ks : specular color
			else if (line[4] == 'K' && line[5] == 's' && line[6] == ' ')
			{
				std::string path = originPath + line.substr(7, len - 7);
				m_mtls[mtlCnt - 1].map_Ks = path;
			}
		}
	}

	file.close();
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


	// ???? ???? (v)
	tmpVtx.x = m_objs[num].v[v_id - 1].d[0];
	tmpVtx.y = m_objs[num].v[v_id - 1].d[1];
	tmpVtx.z = m_objs[num].v[v_id - 1].d[2];

	// ???? ???? ???? (vn)
	tmpVtx.nx = m_objs[num].vn[vn_id - 1].d[0];
	tmpVtx.ny = m_objs[num].vn[vn_id - 1].d[1];
	tmpVtx.nz = m_objs[num].vn[vn_id - 1].d[2];

	// ?????? ???? (vt)
	if (vt_id != NULL)									// If it has Textures ?
	{
		m_bIsTexturing = TRUE;

		tmpVtx.u = m_objs[num].vt[vt_id - 1].d[0];
		tmpVtx.v = m_objs[num].vt[vt_id - 1].d[1];
		m_objs[num].fvf = D3DFVF_TEXTUREVERTEX;			// Texture ???? ????, FVF ?? ?????? ?????? ????.
		
	}
	else
	{
		m_bIsTexturing = FALSE;

		tmpVtx.u = 0.0f;
		tmpVtx.v = 0.0f;
		m_objs[num].fvf = D3DFVF_NOTEXTUREVERTEX;			// Texture ???? ????, FVF ?? ??????(D3DFVF_DIFFUSE)???? ????.
	}
	return tmpVtx;
}

VOID CObjMgr::SaveToListIndices(INDEXLIST list)
{
	INDEXLIST::iterator indiceIter;
	INDEXLIST::iterator tmpIter;
	INDEXLIST::iterator checkIter;


	// triangle list
	INDEXLIST list_indices;				
	// triangle strip iterator
	indiceIter = list.begin();
	tmpIter = indiceIter;

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

// TO DO : ?????? list -> list_indices ?? triangle_list ?? ???? ????.
// 	for (; list.end() != indiceIter; indiceIter++)
// 	{
// 		indiceIter = tmpIter;
// 
// 		list_indices.push_back(*indiceIter);
// 		list_indices.push_back(*(++indiceIter));
// 		list_indices.push_back(*(++indiceIter));
// 
// 		tmpIter = indiceIter;
// 		checkIter = indiceIter;
// 		
// 		if (++checkIter != list.end())
// 		{
// 			list_indices.push_back(*indiceIter);
// 			list_indices.push_back(*(--indiceIter));
// 			++indiceIter;
// 			list_indices.push_back(*(++indiceIter));
// 		}
// 		else
// 			break;
// 	}

//	CCW
//	-------------------------
//  0     3 
// 	|\    |   
//	| \   |  
//	|  \  | 
//  |   \ |
//  1     2 
//  -------------------------
// STRIP : { 0, 1, 2, 3 }
// LIST  : { {0,1,2}, {0,2,3} }

	tmpIter++;		// ??????????

	for (; list.end() != indiceIter;)
	{
		checkIter = indiceIter;
		indiceIter = tmpIter;

		if (++checkIter != list.end())
		{
			list_indices.push_back(*list.begin());
			list_indices.push_back(*(tmpIter));
			list_indices.push_back(*(++indiceIter));
			tmpIter = indiceIter;
		}
		else
			break;
	}

	// list_indices -> m_list_indices ?? insert.
	m_list_indices.insert( m_list_indices.end(), list_indices.begin(), list_indices.end() );
	
	list_indices.clear();
}

VOID CObjMgr::SaveToOriginalIndices(INDEXLIST list)
{
	INDEXLIST::iterator indiceIter;
	INDEXLIST::iterator tmpIter;
	INDEXLIST::iterator checkIter;


	// triangle list
	INDEXLIST list_indices;

	indiceIter = list.begin();
	tmpIter = indiceIter;
	tmpIter++;		// ??????????

	for (; list.end() != indiceIter;)
	{
		checkIter = indiceIter;
		indiceIter = tmpIter;

		if (++checkIter != list.end())
		{
			list_indices.push_back(*list.begin());
			list_indices.push_back(*(tmpIter));
			list_indices.push_back(*(++indiceIter));
			tmpIter = indiceIter;
		}
		else
			break;
	}

	m_original_indices.insert(m_original_indices.end(), list_indices.begin(), list_indices.end());

	list_indices.clear();
}

// To Create Obj Buffer. each obj.
VOID CObjMgr::CreateObjBuffer(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}

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

		HRESULT hr = pDriver->CopyObjVertexBuffer(index, &m_verticesList[i][0], m_vtxSize * m_verticesList[i].size());
		if (FAILED(hr))
			return;
	
		// List
		index = pDriver->CreateObjIndexBuffer((m_list_indicesList[i].size()) * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		hr = pDriver->CopyObjIndexBuffer(index, &m_list_indicesList[i][0], m_indexSize * m_list_indicesList[i].size() );
		if (FAILED(hr))
			return;

		// Strip
// 		index = pDriver->CreateObjIndexBuffer(m_indicesList[i].size() * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
// 		hr = pDriver->CopyObjIndexBuffer(index, &m_indicesList[i].begin(), sizeof(m_indicesList[i]));
	}	
	
}

VOID CObjMgr::CreateOriginalObjBuffer(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}

	for (int i = 0; i < m_original_V_List.size(); i++)
	{
		DWORD dwFVF;

		if (m_bIsTexturingList[i])
			dwFVF = D3DFVF_TEXTUREVERTEX;
		else
			dwFVF = D3DFVF_NOTEXTUREVERTEX;

		UINT index = pDriver->CreateObjVertexBuffer(m_original_V_List[i].size() * m_vtxSize, 0, dwFVF, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		HRESULT hr = pDriver->CopyObjVertexBuffer(index, &m_original_V_List[i][0], m_vtxSize * m_original_V_List[i].size());
		if (FAILED(hr))
			return;

		// List
		index = pDriver->CreateObjIndexBuffer((m_original_I_List[i].size()) * m_indexSize, 0, m_vFormat, D3DPOOL_MANAGED);
		if (index == (UINT)-1)
			return;

		hr = pDriver->CopyObjIndexBuffer(index, &m_original_I_List[i][0], m_indexSize * m_original_I_List[i].size());
		if (FAILED(hr))
			return;
	}
}


/* Draw ???? */
VOID CObjMgr::ObjDraw(CDxDriver* pDriver)
{
	if (pDriver == nullptr)
	{
		MessageBox(NULL, TEXT("No driver."), TEXT("Error"), MB_OK);
		return;
	}

	// Culling CCW (??????)
	pDriver->m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);					// ???????? D3DCULL_NONE ?? ????
	pDriver->m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDriver->m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
// 	pDriver->m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
// 	pDriver->m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
// 	pDriver->m_pD3DDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);

	// Drawing
	//pDriver->DrawObjListModel(this);
	//pDriver->DrawObjStripModel(this);
	pDriver->DrawObjOriginalModel(this);
}

// Create Obj Texture
BOOL CObjMgr::LoadTexture(CDxDriver* pDriver)
{
	int noFileCnt = 0;
	if (!m_mtls.empty())
	{
		for (int i = 0; i < m_mtls.size(); i++)
		{
			if (!m_mtls[i].map_Kd.empty())
			{
				std::cout << m_mtls[i].map_Kd << std::endl;
				m_mtls[i].texIndex = pDriver->CreateTexture(m_mtls[i].map_Kd.c_str());
				if (m_mtls[i].texIndex == (UINT)-1)
					return FALSE;
			}
			else
			{
				noFileCnt++;
				std::cout << i + 1 << "???? mtl texture ?????? ????????. " << std::endl;
			}

		}
		if (noFileCnt == m_mtls.size())
		{
			std::cout << "No Resource File." << std::endl;
			return FALSE;
		}
	}
	else
		return FALSE;

	return TRUE;
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


std::wstring CObjMgr::StringToWstring(const std::string& str)
{
	int length = (int)str.length() + 1;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), length, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), length, buf, len);
	std::wstring r(buf);
	delete[] buf;

	return r;
}

VOID CObjMgr::Term()
{
	m_vertices.clear();
	m_list_indices.clear();
	m_bIsTexturingList.clear();

	for (int i = 0; i < m_original_V_List.size(); i++)
	{
		m_original_V_List[i].clear();
	}
	m_original_V_List.clear();

	for (int i = 0; i < m_original_I_List.size(); i++)
	{
		m_original_I_List[i].clear();
	}
	m_original_I_List.clear();

	for (int i = 0; i < m_indicesList.size(); i++)
	{
		m_indicesList[i].clear();
	}
	m_indicesList.clear();

	for (int i = 0; i < m_list_indicesList.size(); i++)
	{
		m_list_indicesList[i].clear();
	}
	m_list_indicesList.clear();

	for (int i = 0; i < m_objs.size(); i++)
	{
		m_objs[i].name.clear();
		m_objs[i].v.clear();
		m_objs[i].vt.clear();
		m_objs[i].vn.clear();
		m_objs[i].f.clear();
	}
	m_objs.clear();

	std::unordered_map<std::string, DWORD>::iterator u_mapIter;
	for (u_mapIter = m_uMap.begin(); u_mapIter != m_uMap.end(); )
	{
		if (u_mapIter->second != NULL)
		{
			m_uMap.erase(++u_mapIter);
		}
		else
			++u_mapIter;
	}
	m_uMap.clear();
}






