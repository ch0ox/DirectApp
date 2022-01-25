#pragma once
/*------------------------------------------------------------------------*/
//								CDxDriver.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_DxDriver_H
#define __CHAE_DxDriver_H

class CDxDriver;
class App;
class CKeyboard;
class CDraw;
class CMouse;
class CObjMgr;
class CButton;

#include <windows.h>
#include <vector>
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


class CDxDriver
{
	typedef std::vector<LPDIRECT3DVERTEXBUFFER9>			VERTEXBUFFERLIST;
	typedef std::vector<LPDIRECT3DVERTEXBUFFER9>::iterator	VERTEXBUFFERLISTItr;
	typedef std::vector<LPDIRECT3DINDEXBUFFER9>				INDEXBUFFERLIST;
	typedef std::vector<LPDIRECT3DINDEXBUFFER9>::iterator	INDEXBUFFERLISTItr;
	typedef std::vector<LPDIRECT3DTEXTURE9>					TEXTURELIST;
	typedef std::vector<LPDIRECT3DTEXTURE9>::iterator		TEXTURELISTItr;

public:
	explicit CDxDriver(App* app);
	virtual ~CDxDriver();

	BOOL Initialize(HWND);
	BOOL Render(class CDxInput* pInput);
	VOID Term();
	INT ExitMessageBox();
	VOID ChangeDisplayMode(int mode);
	VOID DeviceLostRecovery();
	VOID InputRender(class CDxInput* pInput);

	UINT CreateTexture(const char* psz);
	UINT CreateObjVertexBuffer(UINT length, DWORD usage, DWORD fvf, D3DPOOL pool);
	UINT CreateObjIndexBuffer(UINT length, DWORD usage, D3DFORMAT format, D3DPOOL pool);
	HRESULT CopyObjVertexBuffer(UINT index, const void* p_src, int size);
	HRESULT CopyObjIndexBuffer(UINT index, const void* p_src, int p_size);

	VOID DrawRect(CButton* pButton);
	VOID DrawObjStripModel(CObjMgr* p_ObjMgr);
	VOID DrawObjListModel(CObjMgr* p_ObjMgr);

	VOID SetLight(BOOL bLight);
	VOID SetMaterial();
	VOID SetPosition(D3DXVECTOR3 pos);
	HRESULT SetTexture(UINT texture);
	VOID SetWorldMatrix(D3DXMATRIXA16& matWorld);
	VOID SetProjMatrix(D3DXMATRIXA16& matProj);
	VOID SetCameraMatrix(D3DXMATRIXA16& matView, D3DXVECTOR3 p_eye, D3DXVECTOR3 p_at, D3DXVECTOR3 p_up);

	D3DXVECTOR3 GetEye() { return m_eye; }
	D3DXVECTOR3 GetAt() { return m_at; }
	D3DXVECTOR3 GetUp() { return m_up; }
	VOID SetEye(D3DXVECTOR3 p_eye) { m_eye = p_eye; }
	VOID SetAt(D3DXVECTOR3 p_at) { m_at = p_at; }
	VOID SetUp(D3DXVECTOR3 p_up) { m_up = p_up; }


	CDraw* m_pDraw;
	App* m_pApp;

	LPDIRECT3D9					m_pD3D = nullptr;
	LPDIRECT3DDEVICE9			m_pD3DDevice = nullptr;
	D3DPRESENT_PARAMETERS		m_d3dpp = { NULL };
	HWND						m_hWnd = nullptr;
	LPD3DXFONT					m_pFont = nullptr;							// 폰트
	D3DXFONT_DESC				m_pDesc = { NULL };
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;							// 정점 버퍼
	LPDIRECT3DVERTEXBUFFER9		m_pVB_Btn = nullptr;						// Button 정점 버퍼
	LPDIRECT3DVERTEXBUFFER9		m_pVB_Tri = nullptr;						// Triangle 정점 버퍼
	LPDIRECT3DINDEXBUFFER9		m_pIB_Tri = nullptr;						// 인덱스 버퍼
	VERTEXBUFFERLIST			m_pVertexBufferList;
	INDEXBUFFERLIST				m_pIndexBufferList;
	TEXTURELIST					m_pTextureList;

	BOOL						WindowMode = TRUE;
	BOOL						m_bLostDevice = FALSE;

	D3DXMATRIXA16				m_matWorld;
	D3DXMATRIXA16				m_matView;
	D3DXMATRIXA16				m_matProj;
	FLOAT						m_fAngle = 0.0f;
	FLOAT						m_fScale = 1.0f;
	D3DXVECTOR3					m_pos;
	D3DXVECTOR3					m_eye, m_at, m_up;


private:
	VOID						Drawing();
	double						m_duringTime = 0;


protected:
	HRESULT						InitD3D();
	HRESULT						InitVB();
	HRESULT						InitGeometry();

};




#endif //__CHAE_DxDriver_H















