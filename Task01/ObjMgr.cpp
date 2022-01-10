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

BOOL CObjMgr::ObjLoad(std::ifstream& file)
{
	int vertexNum = 0;						// v - vertex
	int faceNum = 0;						// f - face

	std::string line = "";
	CObj tmpObj;
	int objCnt = 0;

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
		if (line[0] == 'o' && line[1] == ' ')
		{
			tmpObj.name = line.substr(START_CONTEXT, len - START_CONTEXT);
			objs.push_back(tmpObj);

			if (objCnt > 0)
			{
				prev_v += objs[objCnt - 1].v.size();
				prev_vt += objs[objCnt - 1].vt.size();
				prev_vn += objs[objCnt - 1].vn.size();
			}
			objCnt += 1;
		}

		// v
		if (line[0] == 'v' && line[1] == ' ')							// vertex ?
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT, len - START_CONTEXT).c_str(), (char*)" ");
			p3f.d = { vf[0], vf[1], vf[2] };
			objs[objCnt - 1].v.push_back(p3f);
			vertexNum += 1;
		}

		// vt
		else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')	// vertex texture ?
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			p2f.d = { vf[0], vf[1] };
			objs[objCnt - 1].vt.push_back(p2f);
		}

		// vn
		else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')	// vertex normal ?
		{
			vf = StrtokFloat((char*)line.substr(START_CONTEXT_else, len - START_CONTEXT_else).c_str(), (char*)" ");
			p3f.d = { vf[0], vf[1], vf[2] };
			objs[objCnt - 1].vn.push_back(p3f);
		}

		//f
		else if (line[0] == 'f' && line[1] == ' ')						// face ?
		{
			faceNum += 1;
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
			objs[objCnt - 1].f.push_back(tmpFace);
		}
	}


	return TRUE;
}


VOID CObjMgr::ObjDraw(CObjMgr obj)
{
	float x, y, z, nx, ny, nz;
	int v_id, vt_id, vn_id;

	// num : obj number
	for (int num = 0; num < obj.objs.size(); num++)
	{
		int f = obj.objs[num].f.size();

		for (int i = 0; i < f; i++)
		{
			int points = obj.objs[num].f[i].v_pairs.size();
			for (int j = 0; j < points; j++)
			{
				v_id = obj.objs[num].f[i].v_pairs[j].d[0];
				vt_id = obj.objs[num].f[i].v_pairs[j].d[1];
				vn_id = obj.objs[num].f[i].v_pairs[j].d[2];

				// ÁÂÇ¥
				x = obj.objs[num].v[v_id - 1].d[0];
				y = obj.objs[num].v[v_id - 1].d[1];
				z = obj.objs[num].v[v_id - 1].d[2];

				// ¹ý¼± º¤ÅÍ ÁÂÇ¥
				nx = obj.objs[num].v[vn_id - 1].d[0];
				ny = obj.objs[num].v[vn_id - 1].d[1];
				nz = obj.objs[num].v[vn_id - 1].d[2];
			}
		}
	}

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




// To Save Object Data.
/*------------------------------------------------------------------------*/
//								CObjData.cpp
/*------------------------------------------------------------------------*/
CObjData::CObjData()
{

}

CObjData::~CObjData()
{

}