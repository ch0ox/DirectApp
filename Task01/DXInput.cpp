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

VOID CDxInput::GetDevice()
{
	pKey->GetDevice();
	pMouse->GetDevice();
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

	m_inputMgr->CreateDevice(GUID_SysMouse, &(pMouse->m_pMouseDevice), 0);
	if (!pMouse->m_pMouseDevice)
	{
		MessageBox(hwnd, TEXT("Mouse Create Failed!"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	pMouse->m_pMouseDevice->SetDataFormat(&c_dfDIMouse2);
	pMouse->m_pMouseDevice->SetCooperativeLevel(hwnd, mouseFlags);
	pMouse->m_pMouseDevice->Acquire();

	// Keyboard Connect
	pKey = new CKeyboard();

	m_inputMgr->CreateDevice(GUID_SysKeyboard, &(pKey->m_pKeyDevice), 0);
	if (!pKey->m_pKeyDevice)
	{
		MessageBox(hwnd, TEXT("Keyboard Create Failed!"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	pKey->m_pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	pKey->m_pKeyDevice->SetCooperativeLevel(hwnd, keyboardFlags);
	pKey->m_pKeyDevice->Acquire();

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
