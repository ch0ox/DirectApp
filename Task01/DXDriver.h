#pragma once
/*------------------------------------------------------------------------*/
//								CDxDriver.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_DxDriver_H
#define __CHAE_DxDriver_H


class CKeyboard;
class CDraw;
class CMouse;

#include <windows.h>
#include <iostream>
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>
#include <profileapi.h>					// FPS
#include <d3dx9core.h>					// Text
#include <DxErr.h>	
#include <D3dx9tex.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")

#define windowMode		1
#define fullScreenMode	2

#define down 1
#define up 2
#define move 3

// ������â ũ��
#define nWidth		800
#define nHeight		600

// DirectX ���� ��ü���� �����ϴ� ��ũ�� �Լ�
#define SAFE_RELEASE(p)	{if(p) {(p)->Release();	(p)= NULL;}}

// WinMain �ּ� ��ġ���� ���� ����
#pragma warning (disable: 28251)


class CDxDriver {
public:
	CDxDriver();
	virtual ~CDxDriver();

	BOOL Initialize(HWND);
	BOOL Render();
	VOID Term();

public:
	CDraw* pDraw;
	CMouse* pMouse;

protected:
	HRESULT InitD3D();
	HRESULT InitVB();
	HRESULT InitGeometry();

	//	virtual VOID Render();

public:
	INT ExitMessageBox();
	VOID ChangeDisplayMode(int mode);
	VOID DeviceLostRecovery();

private:
	VOID Drawing();

public:
	LPDIRECT3D9					pD3D = nullptr;
	LPDIRECT3DDEVICE9			pd3dDevice = nullptr;
	D3DPRESENT_PARAMETERS		d3dpp = { NULL };
	HWND						g_hWnd = nullptr;

public:
	LPD3DXFONT					g_pFont = nullptr;							// ��Ʈ
	D3DXFONT_DESC				g_pDesc = { NULL };
	LPDIRECT3DVERTEXBUFFER9		g_pVB = nullptr;							// ���� ����
	LPDIRECT3DVERTEXBUFFER9		g_pVB_Btn = nullptr;						// Button ���� ����
	LPDIRECT3DVERTEXBUFFER9		g_pVB_Tri = nullptr;						// Triangle ���� ����
	LPDIRECT3DINDEXBUFFER9		g_pIB = nullptr;							// �ε��� ����
	LPDIRECT3DTEXTURE9			g_pTexture = nullptr;						// ����� �ؽ���

	BOOL						WindowMode = TRUE;

	BOOL						g_bLostDevice = FALSE;

private:

	double						duringTime = 0;

private:

};


#endif //__CHAE_DxDriver_H















