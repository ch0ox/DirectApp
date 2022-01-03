#pragma once
/*------------------------------------------------------------------------*/
//								MouseMgr.h
/*------------------------------------------------------------------------*/
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
	CMouse();
	virtual ~CMouse();

	//	LPDIRECTINPUT8				g_pDI;					// DirectInput 객체
	//	LPDIRECTINPUTDEVICE8		g_pMouse;				// 마우스 디바이스

private:
	CDxDriver* g_pDriver = nullptr;

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

