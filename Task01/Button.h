#pragma once
/*------------------------------------------------------------------------*/
//								CButton.h
/*------------------------------------------------------------------------*/
#ifndef __CHAE_Button_H
#define __CHAE_Button_H

#include <string>
#include "Draw.h"

class CMouse;

class CButton
{
public:
	CButton();
	CButton(char*, char*, char*, char*);

	virtual ~CButton();
	VOID LinkD3D(CDxDriver* pDriver);

public:
	HRESULT CreateTexture(LPDIRECT3DDEVICE9);
	VOID SetTexture();
	VOID ButtonAction();
	// 	BOOL OnLButtonUp(int x, int y);
	// 	BOOL OnLButtonDown(int x, int y);
	BOOL IsOnMe(int x, int y, bool);

	FLOAT GetScaleX() { return m_scaleX; }
	FLOAT GetScaleY() { return m_scaleY; }
	UINT16 GetWidth() { return m_width; }
	UINT16 GetHeight() { return m_height; }
	UINT16 GetWideWidth() { return (UINT16)((FLOAT)GetWidth() * GetScaleX()); }
	UINT16 GetWideHeight() { return (UINT16)((FLOAT)GetHeight() * GetScaleY()); }
	FLOAT GetPosX() { return m_posX; }
	FLOAT GetPosY() { return m_posY; }
	VOID SetWidePos(float posX, float posY) { m_widePosX = posX * m_scaleX; m_widePosY = posY * m_scaleY; }
	VOID SetScale(float scaleX, float scaleY);
	VOID SetWideSize(UINT16 width, UINT16 height) { m_wideWidth = width; m_wideHeight = height; }
	VOID SetWideInit() { m_widePosX = m_posX; m_widePosY = m_posY; m_wideWidth = m_width; m_wideHeight = m_height; }

public:
	LPDIRECT3DTEXTURE9* m_ppTexture;
	LPDIRECT3DTEXTURE9  m_pTexture;
	INT	m_state;

private:
	CDxDriver* g_pDriver;

public:
	char* m_btnName;
	UINT			m_action = -1;
	FLOAT			m_scaleX = 1.0f;
	FLOAT			m_scaleY = 1.0f;
	FLOAT			m_posX;
	FLOAT			m_posY;
	UINT16			m_width;
	UINT16			m_height;

	FLOAT			m_widePosX;
	FLOAT			m_widePosY;
	FLOAT			m_wideWidth;
	FLOAT			m_wideHeight;

	char* m_normal_path;
	char* m_over_path;
	char* m_click_path;

	RHWVERTEX		m_vertex[4];

protected:
	char* m_path;
	int m_textureCount;

private:
	VOID LoadIniFile(char*, char*, char*, char*);

};

#endif //__CHAE_Button_H