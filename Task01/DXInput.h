#pragma once
/*------------------------------------------------------------------------*/
//								Input.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_DxInput_H
#define __CHAE_DxInput_H

#include <windows.h>
#include <dinput.h>
#include "MouseMgr.h"
#include "KeyboardMgr.h"

class App;

class CDxInput
{
public:
	CDxInput();
	virtual ~CDxInput();

	BOOL			Initialize(HINSTANCE hInstance, HWND hwnd);
	VOID			Term();
	BOOL			Render();

	CMouse* GetMouseMgr() const;
	CKeyboard* GetKeyboardMgr() const;

	CMouse* pMouse;
	CKeyboard* pKey;

private:
	LPDIRECTINPUT8	m_inputMgr;
	HWND			m_hWnd = nullptr;



};	

#endif //__CHAE_DxInput_H