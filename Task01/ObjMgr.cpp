// To Parse Object File.
/*------------------------------------------------------------------------*/
//								CObjMgr.cpp
/*------------------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "ObjMgr.h"
#include "DXDriver.h"
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
	BOOL bRet = FALSE;


	// Read 1
/*
	FILE* objFile;
	if (fopen_s(&objFile, objFileName, "rt") != 0)				// Read Text mode
	{
		MessageBox(NULL, TEXT("File Open Failed! -_-"), TEXT("Oooooops"), MB_OK);
		return FALSE;
	}

	if (objFile == NULL)	return FALSE;


	// To Count v, f num. And To Store obj name.

	CHAR line[256];
	while (fscanf_s(objFile, "%s", line, sizeof(line)) > 0)
	{
		if (line[0] == 'o' && line[1] == '\0')				// new Obj name
		{
			tmpObj.name;
		}

		if (line[0] == 'v' && line[1] == '\0')				// vertex ?
		{
			vertexNum += 1;
		}
		else if (line[0] == 'f' && line[1] == '\0')			// face ?
		{
			faceNum += 1;
		}

		memset(line, '\0', sizeof(line));
	}
*/
	//std::ifstream file;

	INT vertexNum = 0;						// v - vertex
	INT faceNum = 0;						// f - face

	std::string line = "";
	CObj tmpObj;
	INT objCnt = 0;

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
			objCnt += 1;
			tmpObj.name = line.substr(START_CONTEXT, len - START_CONTEXT);
			objs.push_back(tmpObj);
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
				p3i.d = { vi[0], vi[1], vi[2] };
				tmpFace.v_pairs.push_back(p3i);
			}
			objs[objCnt - 1].f.push_back(tmpFace);
		}

	}
	

	return bRet;
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