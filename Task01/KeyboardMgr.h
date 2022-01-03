#pragma once
/*------------------------------------------------------------------------*/
//								KeyboardMgr.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_KeyboardMgr_H
#define __CHAE_KeyboardMgr_H

class CDxDriver;
class CDraw;

class CKeyboard
{
public:
	CKeyboard();
	virtual ~CKeyboard();

private:
	CDxDriver* g_pDriver = nullptr;

public:
	VOID KeyManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, CDxDriver* pDriver);

private:


protected:

};

#endif //__CHAE_KeyboardMgr_H