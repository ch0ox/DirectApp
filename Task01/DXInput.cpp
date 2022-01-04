/*------------------------------------------------------------------------*/
//								Input.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DXInput.h"
#include "MouseMgr.h"
#include "KeyboardMgr.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

CDxInput::CDxInput() : m_inputMgr(nullptr), pMouse(nullptr), pKey(nullptr)
{

}

CDxInput::~CDxInput()
{
	Term();
}

BOOL CDxInput::Initialize(HINSTANCE hInstance, HWND hwnd, DWORD keyboardFlags, DWORD mouseFlags)
{
	m_hWnd = hwnd;

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_inputMgr), nullptr)))
	{
		MessageBox(hwnd, TEXT("Failed Input"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}

	// Mouse Connect
	pMouse = new CMouse();

	m_inputMgr->CreateDevice(GUID_SysMouse, &(pMouse->m_pMouse), 0);
	if (!pMouse->m_pMouse)
	{
		MessageBox(hwnd, TEXT("Mouse Create Failed!"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	pMouse->m_pMouse->SetDataFormat(&c_dfDIMouse2);
	pMouse->m_pMouse->SetCooperativeLevel(hwnd, mouseFlags);
	pMouse->m_pMouse->Acquire();

	// Keyboard Connect
	pKey = new CKeyboard();

	m_inputMgr->CreateDevice(GUID_SysKeyboard, &(pKey->m_pKeyboard), 0);
	if (!pKey->m_pKeyboard)
	{
		MessageBox(hwnd, TEXT("Keyboard Create Failed!"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	pKey->m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	pKey->m_pKeyboard->SetCooperativeLevel(hwnd, keyboardFlags);
	pKey->m_pKeyboard->Acquire();

	return TRUE;
}

BOOL CDxInput::Render()
{
	// Keyboard , Mouse -> Update

	return TRUE;
}

VOID CDxInput::Term()
{
	if (pKey)
	{
		delete pKey;
		pKey = nullptr;
	}

	if (pMouse)
	{
		delete pMouse;
		pMouse = nullptr;
	}

	if (m_inputMgr)
	{
		m_inputMgr->Release();
		m_inputMgr = nullptr;
	}
}
