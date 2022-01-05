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

	BOOL			Initialize(HINSTANCE hInstance, HWND hwnd, DWORD keyboardFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DWORD mouseFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	VOID			Term();
	VOID			GetDevice();
	BOOL			Render();

	CMouse*			GetMouseMgr() const			{ return pMouse; }
	CKeyboard*		GetKeyboardMgr() const		{ return pKey; }

	HWND			GetHwnd() const				{ return m_hWnd; }

	CMouse*			pMouse;
	CKeyboard*		pKey;

private:
	LPDIRECTINPUT8	m_inputMgr = nullptr;
	HWND			m_hWnd = nullptr;



};	

#endif //__CHAE_DxInput_H