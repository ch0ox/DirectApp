#pragma once
/*------------------------------------------------------------------------*/
//								MouseMgr.h
/*------------------------------------------------------------------------*/
#ifndef __CHAE_MouseMgr_H
#define __CHAE_MouseMgr_H

#include <dinput.h>
#include <vector>
#define idle 0
#define over 1
#define click 2


class CDxDriver;
class Button;
class CButton;

class CMouse
{
public:

	CMouse();
	virtual ~CMouse();

	POINT								ClientCursorPos(HWND hWnd);
	BOOL								IsMouseDown(int button);
	VOID								GetDevice();
	VOID								ShutDown();

	INT									boxState = idle;
	LPDIRECTINPUTDEVICE8				m_pMouseDevice = nullptr;				// 마우스 디바이스
	DIMOUSESTATE2						m_mouseState;

private:

	std::vector<CButton*>				m_btnVector;
	std::vector<CButton*>::iterator		m_btnIter;

protected:
	POINT		m_MousePt;
};

#endif // __CHAE_MouseMgr_H