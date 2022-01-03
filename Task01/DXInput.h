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

	CMouse* pMouse;
	CKeyboard* pKey;

private:
	LPDIRECTINPUT8 inputMgr;
	HWND	g_hWnd = nullptr;

public:
	BOOL Initialize(HINSTANCE hInstance, HWND hwnd);
	VOID Term();
	BOOL Render();


private:
	CMouse* GetMouseMgr() const;
	CKeyboard* GetKeyboardMgr() const;

};	

#endif //__CHAE_DxInput_H