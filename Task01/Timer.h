#pragma once

/*------------------------------------------------------------------------*/
//								CTimer.h
/*------------------------------------------------------------------------*/


#ifndef __CHAE_Timer_H
#define __CHAE_Timer_H

#include <windows.h>
#include <mmsystem.h>
#include <D3dx9tex.h>

class DxDriver;

#define MAX_COUNT 60

class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	VOID			Tick(float fLockFPS = 60.0f);
	FLOAT			GetTimeElapsed() const;
	ULONG			GetFrameRate(LPTSTR lpszString = NULL) const;
	ULONG			GetFPS() const { return m_frameRate; };
// 	BOOL			InitFont(CDxDriver* pDriver);
// 	VOID			DrawFPS(CDxDriver* pDriver);


private:
	BOOL m_perfHardware = TRUE;					// TRUE : QueryPerformanceCounter .  FALSE : timeGetTime .
	FLOAT m_timeScale = 0;						// Query Func -> Seconds
	FLOAT m_timeElapsed = 0;					// 시간 -> Seconds
	INT64 m_currentTime = 0;					// 현재 시간
	INT64 m_lastTime = 0;						// 이전 시간
	INT64 m_perfFreq = 0;						// 주파수 . QueryPerformanceFrequency .

	FLOAT m_frameTime[MAX_COUNT] = { 0, };		// 초당 프레임 수
	ULONG m_sampleCount = 0;					// 현재 시간 샘플 수

	unsigned long m_frameRate = 0;				// Tick Func 호출 횟수
	unsigned long m_fpsFrameCount = 0;			// 해당 초 동안 발생한 총 프레임 수. -> m_frameRate 에 저장.
	FLOAT m_fpsTimeElapsed = 0;					// 처리 중인 현재 초가 시작된 후 경과한 시간.
	UINT m_font;
};

#endif //__CHAE_Timer_H