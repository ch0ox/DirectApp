/*------------------------------------------------------------------------*/
//								CTimer.cpp
/*------------------------------------------------------------------------*/

#include "WinApp.h"
#include "Timer.h"
#include "main.h"
#include "tchar.h"
#include "windows.h"
#include <Windows.h>
#include <Winnt.h>
#include "DXDriver.h"

#pragma comment(lib, "winmm.lib")
#pragma warning(disable : 4996)

CTimer::CTimer()
{
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &m_perfFreq))
	{
		m_perfHardware = TRUE;
		QueryPerformanceCounter((LARGE_INTEGER*)&m_lastTime);
		m_timeScale = 1.0f / m_perfFreq;
	}
	else							// QueryPerformance 함수 사용 불가능이면 timeGetTime 으로 대체 ( m_perfHardware = False )
	{
		m_perfHardware = FALSE;
		m_lastTime = timeGetTime();
		m_timeScale = 0.001f;
	}
	m_frameRate = 0;
	m_sampleCount = 0;
	m_fpsFrameCount = 0;
	m_fpsTimeElapsed = 0.0f;
	m_timeElapsed = 0.0f;
}

CTimer::~CTimer()
{

}

VOID CTimer::Tick(float fLockFPS)
{
	float fTimeElapsed;
	
	// Get Current Time
	if (m_perfHardware)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_currentTime);
	}
	else
		m_currentTime = timeGetTime();

	fTimeElapsed = (m_currentTime - m_lastTime) * m_timeScale;

	if (fLockFPS > 0.0f)
	{
		// Get Current Time Loop
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			if (m_perfHardware)
			{
				QueryPerformanceCounter((LARGE_INTEGER*)&m_currentTime);
			}
			else
			{
				m_currentTime = timeGetTime();
			}

			fTimeElapsed = (m_currentTime - m_lastTime) * m_timeScale;
		}
	}

	// Save current frame time
	m_lastTime = m_currentTime;

	// fabsf : floating - point absolute value function
	if (fabsf(fTimeElapsed - m_timeElapsed) < 1.0f)
	{
		// Wrap FIFO frame time buffer.
		memmove(&m_frameTime[1], m_frameTime, (MAX_COUNT - 1) * sizeof(float));
		m_frameTime[0] = fTimeElapsed;

		if (m_sampleCount < MAX_COUNT)
			m_sampleCount++;
	}

	// Calculate frame rate
	m_fpsFrameCount++;

	m_fpsTimeElapsed += m_timeElapsed;
	
	if (m_fpsTimeElapsed > 1.0f)
	{
		// Save.
		m_frameRate = m_fpsFrameCount;
		// Reset m_fpsFrameCount and m_fpsTimeElapsed to zero.
		m_fpsFrameCount = 0;
		m_fpsTimeElapsed = 0.0f;
	}

	m_timeElapsed = 0.0f;
	for (ULONG i = 0; i < m_sampleCount; i++)
	{
		m_timeElapsed += m_frameTime[i];
	}

	if (m_sampleCount > 0)
		m_timeElapsed /= m_sampleCount;
}

FLOAT CTimer::GetTimeElapsed() const
{
	return m_timeElapsed;
}

ULONG CTimer::GetFrameRate(LPTSTR lpszString) const
{
	if (lpszString)
	{
		// Copy frame Rate to string.
		_itot(m_frameRate, lpszString, 10);		//  value, buffer, radix
		_tcscat(lpszString, TEXT("FPS"));
	}
	return m_frameRate;
}

