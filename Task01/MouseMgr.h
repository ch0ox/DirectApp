#pragma once
/*------------------------------------------------------------------------*/
//								MouseMgr.h
/*------------------------------------------------------------------------*/
#ifndef __CHAE_MouseMgr_H
#define __CHAE_MouseMgr_H

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

	//	LPDIRECTINPUT8				g_pDI;					// DirectInput ��ü
	//	LPDIRECTINPUTDEVICE8		g_pMouse;				// ���콺 ����̽�

private:
	CDxDriver* m_pDriver = nullptr;

public:
	VOID		LinkD3D(CDxDriver* pDriver);
	VOID		LinkButton(std::vector<CButton*>& v);
	POINT		ClientCursorPos();
	//INT MouseManager();
	VOID		MouseManager(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, INT flag);
	BOOL		InButton(HWND hWnd, CButton* pButton);
	INT			boxState = idle;

protected:
	POINT		m_MousePt;

private:
	BOOL		OnMouseMove(int x, int y);
	std::vector<CButton*>				btnVector;
	std::vector<CButton*>::iterator		btnIter;

};

#endif // __CHAE_MouseMgr_H