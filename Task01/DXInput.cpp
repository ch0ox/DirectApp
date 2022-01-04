/*------------------------------------------------------------------------*/
//								Input.cpp
/*------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DXInput.h"

CDxInput::CDxInput() : m_inputMgr(nullptr), pMouse(nullptr), pKey(nullptr)
{

}

CDxInput::~CDxInput()
{
	Term();
}

BOOL CDxInput::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	m_hWnd = hwnd;

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_inputMgr), nullptr)))
	{
		return FALSE;
	}

	pMouse = new CMouse();
	pKey = new CKeyboard();

	return TRUE;
}

BOOL CDxInput::Render()
{
	// Keyboard , Mouse -> Update

	return TRUE;
}

VOID CDxInput::Term()
{
	if (m_inputMgr)
	{
		m_inputMgr->Release();
		m_inputMgr = nullptr;
	}

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
}

CMouse* CDxInput::GetMouseMgr() const
{
	return pMouse;
}

CKeyboard* CDxInput::GetKeyboardMgr() const
{
	return pKey;
}