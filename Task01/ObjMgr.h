#pragma once

#ifndef __CHAE_ObjMgr_H
#define __CHAE_ObjMgr_H

#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#define START_CONTEXT 2
#define START_CONTEXT_else 3

class CPoint2f
{
public:
	std::vector <FLOAT> d{ 0,0 };
};

class CPoint3f
{
public:
	std::vector <FLOAT> d{ 0,0,0 };
};

class CPoint3i
{
public:
	std::vector <INT> d{ 0,0,0 };
};

class CFace
{
public:
	std::vector<CPoint3i> v_pairs;
};

class CObj
{
public:
	std::string				name;
	std::vector<CPoint3f>	v;
	std::vector<CPoint2f>	vt;
	std::vector<CPoint3f>	vn;
	std::vector<CFace>		f;
};

/*------------------------------------------------------------------------*/
//								CObjMgr.h
/*------------------------------------------------------------------------*/

class CObjMgr
{
public:
	CObjMgr();
	virtual ~CObjMgr();

	BOOL ObjLoad(std::ifstream&);
	VOID ObjDraw();

	std::vector <CObj> objs;								// Objects Vector
	std::vector <FLOAT> StrtokFloat(char*, char*);
	std::vector <std::string> StrtokString(char*, char*);
	std::vector <INT> StrtokInt(char*, char*);

private:


};




typedef struct VECTOR3
{
	FLOAT x, y, z;
}vec3;

typedef struct VECTOR2
{
	FLOAT u, v;
}vec2;


/*------------------------------------------------------------------------*/
//								CObjData.h
/*------------------------------------------------------------------------*/

class CObjData
{
public:
	CObjData();
	virtual ~CObjData();

private:


};


#endif //__CHAE_ObjMgr_H