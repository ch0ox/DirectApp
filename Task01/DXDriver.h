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
	INT ExitMessageBox();
	VOID ChangeDisplayMode(int mode);
	VOID DeviceLostRecovery();

	CDraw* pDraw;
	CMouse* pMouse;

	LPDIRECT3D9					m_pD3D = nullptr;
	LPDIRECT3DDEVICE9			m_pD3DDevice = nullptr;
	D3DPRESENT_PARAMETERS		m_d3dpp = { NULL };
	HWND						m_hWnd = nullptr;
	LPD3DXFONT					m_pFont = nullptr;							// ��Ʈ
	D3DXFONT_DESC				m_pDesc = { NULL };
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;							// ���� ����
	LPDIRECT3DVERTEXBUFFER9		m_pVB_Btn = nullptr;						// Button ���� ����
	LPDIRECT3DVERTEXBUFFER9		m_pVB_Tri = nullptr;						// Triangle ���� ����
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;							// �ε��� ����
	LPDIRECT3DTEXTURE9			m_pTexture = nullptr;						// ����� �ؽ���


	BOOL						WindowMode = TRUE;

	BOOL						m_bLostDevice = FALSE;

private:
	VOID						Drawing();
	double						duringTime = 0;

protected:
	HRESULT						InitD3D();
	HRESULT						InitVB();
	HRESULT						InitGeometry();

};




#endif //__CHAE_DxDriver_H















