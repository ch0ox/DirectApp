#pragma once
/*------------------------------------------------------------------------*/
//								WinApp.h
/*------------------------------------------------------------------------*/

#ifndef __CHAE_App_H
#define __CHAE_App_H


#include <windows.h>
#include <iostream>
#include <d3d9.h>
#include <dinput.h>
#include <tchar.h>
#include <profileapi.h>					// FPS
#include <d3dx9core.h>					// Text
#include <DxErr.h>	
#include <D3dx9tex.h>

#include "main.h"

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

/*------------------------------------------------------------------------*/

#include "DXDriver.h"
#include "DXInput.h"

class CDxDriver;
class CDxInput;
class CButton;
class CTimer;

/*------------------------------------------------------------------------*/
//								
/*------------------------------------------------------------------------*/

class App
{
public:
	App();
	virtual ~App();

	BOOL Initialize();
	VOID Run();
	VOID InitWindow();
	LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam);
	LRESULT CALLBACK MsgHandelr(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam);
	VOID Term();
	BOOL Render();

	BOOL WindowMode = TRUE;
	HWND g_hWnd = nullptr;

	CButton* GetButton() const { return m_pButton; }
	CTimer* GetTimer() const { return m_pTimer; }

private:
	VOID ShutDownWindow();

	HINSTANCE		g_hInstance = nullptr;

	// fps
	// 삭제 예정
	INT	m_fps = 0;
	int	FPS = 60;
	double clocks = 1;
	double duringTime = 0;
	LARGE_INTEGER m_nowframe = { 0 };
	LARGE_INTEGER m_lastTime = { 0 };
	LARGE_INTEGER m_currentTime = { 0 };
	LARGE_INTEGER m_frequency = { 0 };

protected:
	CDxDriver* m_pDXDriver = nullptr;
	CDxInput* m_pDxInput = nullptr;
	CButton* m_pButton = nullptr;
	CTimer* m_pTimer = nullptr;
};


#endif //__CHAE_App_H








