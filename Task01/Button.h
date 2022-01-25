#pragma once
/*------------------------------------------------------------------------*/
//								CButton.h
/*------------------------------------------------------------------------*/
#ifndef __CHAE_Button_H
#define __CHAE_Button_H

#include <string>
#include <vector>
#include "Draw.h"

class CMouse;

class CButton
{
public:
	CButton();
	CButton(int action, float, float, UINT16, UINT16);

	virtual ~CButton();

	VOID SetTexture(CDxDriver* pDriver);
	BOOL IsOnMe(int x, int y, bool);
	VOID RectangleInit(CDxDriver* pDriver);
	VOID TextureInit(CDxDriver* pDriver);
	VOID Check(bool, bool bState, int x, int y);

	FLOAT GetScaleX() { return m_scaleX; }
	FLOAT GetScaleY() { return m_scaleY; }
	UINT16 GetWidth() { return m_width; }
	UINT16 GetHeight() { return m_height; }
	UINT16 GetWideWidth() { return (UINT16)((FLOAT)GetWidth() * GetScaleX()); }
	UINT16 GetWideHeight() { return (UINT16)((FLOAT)GetHeight() * GetScaleY()); }
	FLOAT GetPosX() { return m_posX; }
	FLOAT GetPosY() { return m_posY; }
	INT GetState() { return m_state; }
	UINT GetAction() { return m_action; }
	VOID SetWidePos(float posX, float posY) { m_widePosX = posX * m_scaleX; m_widePosY = posY * m_scaleY; }
	VOID SetScale(float scaleX, float scaleY);
	VOID SetWideSize(UINT16 width, UINT16 height) { m_wideWidth = width; m_wideHeight = height; }
	VOID SetWideInit() { m_widePosX = m_posX; m_widePosY = m_posY; m_wideWidth = m_width; m_wideHeight = m_height; }
	VOID SetTexPath(char* normalPath, char* overPath, char* clickPath);

	INT					m_state = idle;

	char* m_btnName = { 0, };
	UINT				m_action = 0;
	FLOAT				m_scaleX = 1.0f;
	FLOAT				m_scaleY = 1.0f;
	FLOAT				m_posX = 0;
	FLOAT				m_posY = 0;
	UINT16				m_width = 0;
	UINT16				m_height = 0;

	FLOAT				m_widePosX = 0;
	FLOAT				m_widePosY = 0;
	FLOAT				m_wideWidth = 0;
	FLOAT				m_wideHeight = 0;

	RHWVERTEX			m_vertex[4];

private:
	UINT				m_iTexture[3] = { 0, };

	std::string			m_normal_path;
	std::string			m_over_path;
	std::string			m_click_path;

protected:


};

#endif //__CHAE_Button_H