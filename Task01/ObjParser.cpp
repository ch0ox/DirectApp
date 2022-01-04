/*------------------------------------------------------------------------*/
//								CObjParser.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "ObjParser.h"
#include <iostream>
#include <string>
#include <fstream>

CObj::CObj()
{

}

CObj::~CObj()
{

}

BOOL CObj::ObjParse()
{
	bool bRet = false;

	char count[100];
	int vertexNum = 0;						// v - vertex
	int vertexNormalNum = 0;				// vn - vertex normal
	int vertexTextureNum = 0;				// vt - vertex texture
	int faceNum = 0;						// f - face

	FILE* fileObj;
	char* objFileName = { 0, };

// 	if (fopen_s(&fileObj, objFileName, "rt") != 0)
// 		return FALSE;

	



	return bRet;
}
