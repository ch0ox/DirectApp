// To Parse Object File.
/*------------------------------------------------------------------------*/
//								CObjMgr.cpp
/*------------------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "ObjMgr.h"
#include "DXDriver.h"
#include <fstream>
#include <iostream>

#include <stdio.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

CObjMgr::CObjMgr()
{

}

CObjMgr::~CObjMgr()
{

}

BOOL CObjMgr::ObjLoad(char* objFileName)
{
	BOOL bRet = FALSE;
	INT vertexNum = 0;						// v - vertex
	INT faceNum = 0;						// f - face

	std::string fileLine = "";
	CObj tmpObj;
	INT objCnt = 0;

	// Read 1
	FILE* objFile;
	if (fopen_s(&objFile, objFileName, "rt") != 0)				// Read Text mode
		return FALSE;

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