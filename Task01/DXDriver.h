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

// 윈도우창 크기
#define nWidth		800
#define nHeight		600

// DirectX 관련 객체들을 해제하는 매크로 함수
#define SAFE_RELEASE(p)	{if(p) {(p)->Release();	(p)= NULL;}}

// WinMain 주석 일치하지 않은 오류
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
	LPD3DXFONT					g_pFont = nullptr;							// 폰트
	D3DXFONT_DESC				g_pDesc = { NULL };
	LPDIRECT3DVERTEXBUFFER9		g_pVB = nullptr;							// 정점 버퍼
	LPDIRECT3DVERTEXBUFFER9		g_pVB_Btn = nullptr;						// Button 정점 버퍼
	LPDIRECT3DVERTEXBUFFER9		g_pVB_Tri = nullptr;						// Triangle 정점 버퍼
	LPDIRECT3DINDEXBUFFER9		g_pIB = nullptr;							// 인덱스 버퍼
	LPDIRECT3DTEXTURE9			g_pTexture = nullptr;						// 사용할 텍스쳐

	BOOL						WindowMode = TRUE;

	BOOL						g_bLostDevice = FALSE;

private:

	double						duringTime = 0;

private:

};


#endif //__CHAE_DxDriver_H















