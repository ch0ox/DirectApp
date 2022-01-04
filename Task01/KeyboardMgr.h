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
	VOID						InitializeKeyboard();
	BOOL						KeyDown(char key);

	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	char						m_keyboardCount[256];

private:
	CDxDriver*					m_pDriver = nullptr;

protected:

};

#endif //__CHAE_KeyboardMgr_H