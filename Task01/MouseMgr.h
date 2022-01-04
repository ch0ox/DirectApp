#pragma once
/*------------------------------------------------------------------------*/
//								MouseMgr.h
/*------------------------------------------------------------------------*/
#ifndef __CHAE_MouseMgr_H
#define __CHAE_MouseMgr_H

#include <dinput.h>
#include <vector>
#define idle 1
#define over 2
#define click 3


class CDxDriver;
class Button;
class CButton;

class CMouse
{
public:
// 	enum EState {
// 		idle = 1,
// 		over,
// 		click
// 	};

	CMouse();
	virtual ~CMouse();

	VOID		LinkD3D(CDxDriver* pDriver);
	VOID		LinkButton(std::vector<CButton*>& v);
	POINT		ClientCursorPos();
	//INT MouseManager();
	VOID		MouseManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, INT flag);
	BOOL		InButton(HWND hWnd, CButton* pButton);
	INT			boxState = idle;

	LPDIRECTINPUTDEVICE8		m_pMouse;				// 마우스 디바이스

private:
	CDxDriver* m_pDriver = nullptr;

	BOOL		OnMouseMove(int x, int y);
	std::vector<CButton*>				btnVector;
	std::vector<CButton*>::iterator		btnIter;

protected:
	POINT		m_MousePt;
};

#endif // __CHAE_MouseMgr_H