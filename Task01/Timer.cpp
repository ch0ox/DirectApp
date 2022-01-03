#include "stdafx.h"
#include "WinApp.h"
#include "Timer.h"
#include "main.h"
#include "windows.h"
#include <Windows.h>
#include <Winnt.h>


CTimer::CTimer()
{
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &m_perfFreq))
	{
		m_perfHardware = TRUE;
		QueryPerformanceCounter((LARGE_INTEGER*)&m_lastTime);
		m_timeScale = 1.0f / m_perfFreq;
	}
	else
	{
		m_perfHardware = FALSE;
		m_lastTime = timeGetTime();
		m_timeScale = 0.001f;
	}
}

CTimer::~CTimer()
{

}

VOID CTimer::Tick(float fLockFPS)
{
	float fTimeElapsed;

}

FLOAT CTimer::GetTimeElapsed() const
{
	return m_timeElapsed;
}

unsigned long CTimer::GetFrameRate(LPTSTR lpszString) const
{
	return m_frameRate;
}
