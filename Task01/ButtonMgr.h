#pragma once
/*------------------------------------------------------------------------*/
//								CButtonMgr.h
/*------------------------------------------------------------------------*/
#ifndef __CHAE_ButtonMgr_H
#define __CHAE_ButtonMgr_H

#include "Button.h"
#include "DXDriver.h"
#include <Windows.h>
#include <string>
#include <vector>

class CButton;
class CDxDriver;

class CButtonMgr
{
public:
	CButtonMgr();
	virtual ~CButtonMgr();

	BOOL LoadIniFile(char* szFilePath);
	VOID Initialize(CDxDriver* pDriver);
	VOID Draw(CDxDriver* pDriver);
	VOID Check(CDxDriver* pDriver, CDxInput* pInput, bool bState, int x, int y);
	VOID Action(CDxDriver* pDriver, CDxInput* pInput, int buttonAction);

private:
	std::vector<CButton*>				m_btnVector;
	std::vector<CButton*>::iterator		m_btnIter;

protected:
};

#endif //__CHAE_ButtonMgr_H
