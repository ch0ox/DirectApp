/*------------------------------------------------------------------------*/
//								Input.cpp
/*------------------------------------------------------------------------*/


#include "DXInput.h"
#include "MouseMgr.h"
#include "KeyboardMgr.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

CDxInput::CDxInput() : m_inputMgr(nullptr), m_pMouse(nullptr), pKey(nullptr)
{

}

CDxInput::~CDxInput()
{
	Term();
}

VOID CDxInput::GetDevice()
{
	pKey->GetDevice();
	m_pMouse->GetDevice();
}

BOOL CDxInput::Initialize(HINSTANCE hInstance, HWND hwnd, DWORD keyboardFlags, DWORD mouseFlags)
{
	m_hWnd = hwnd;
	HRESULT result;

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_inputMgr), nullptr)))
	{
		MessageBox(hwnd, TEXT("Failed Input"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	
	// Mouse Connect
	m_pMouse = new CMouse();

	m_inputMgr->CreateDevice(GUID_SysMouse, &(m_pMouse->m_pMouseDevice), 0);
	if (!m_pMouse->m_pMouseDevice)
	{
		MessageBox(hwnd, TEXT("Mouse Create Failed!"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	result = m_pMouse->m_pMouseDevice->SetDataFormat(&c_dfDIMouse2);
	result = m_pMouse->m_pMouseDevice->SetCooperativeLevel(hwnd, mouseFlags);
	result = m_pMouse->m_pMouseDevice->Acquire();

	// Keyboard Connect
	pKey = new CKeyboard();

	m_inputMgr->CreateDevice(GUID_SysKeyboard, &(pKey->m_pKeyDevice), 0);
	if (!pKey->m_pKeyDevice)
	{
		MessageBox(hwnd, TEXT("Keyboard Create Failed!"), TEXT("Failed Msg"), MB_OK);
		return FALSE;
	}
	result = pKey->m_pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	result = pKey->m_pKeyDevice->SetCooperativeLevel(hwnd, keyboardFlags);
	result = pKey->m_pKeyDevice->Acquire();

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
		pKey->ShutDown();
		delete pKey;
		pKey = nullptr;
	}

	if (m_pMouse)
	{
		m_pMouse->ShutDown();
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	if (m_inputMgr)
	{
		m_inputMgr->Release();
		m_inputMgr = nullptr;
	}
}
