#pragma once
/*------------------------------------------------------------------------*/
//								KeyboardMgr.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_KeyboardMgr_H
#define __CHAE_KeyboardMgr_H

#include <dinput.h>

class CDxDriver;
class CDraw;

class CKeyboard
{
public:
	CKeyboard();
	virtual ~CKeyboard();

	VOID KeyManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, CDxDriver* pDriver);

private:
	CDxDriver*					m_pDriver = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard;
	char						m_keyboardCount[256];


protected:

};

#endif //__CHAE_KeyboardMgr_H