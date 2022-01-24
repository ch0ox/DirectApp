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

	BOOL						IsKeyDown(char key);
	VOID						Toggle(CDxDriver* pDriver);
	VOID						GetDevice();
	VOID						ShutDown();

	LPDIRECTINPUTDEVICE8		m_pKeyDevice = nullptr;
	char						m_keyboardState[256];

private:


protected:

};

#endif //__CHAE_KeyboardMgr_H