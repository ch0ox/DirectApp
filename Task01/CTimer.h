#pragma once
#include <windows.h>

#ifndef __CHAE_Timer_H
#define __CHAE_Timer_H


class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	VOID			Tick(float fLockFPS = 0.0f);
	FLOAT			GetTimeElapsed() const;
	unsigned long	GetFrameRate(LPTSTR lpszString = NULL) const;

private:
	BOOL m_perfHardware;			// TRUE : QueryPerformanceCounter .  FALSE : timeGetTime .
	FLOAT m_timeScale;				// Query Func -> Seconds
	FLOAT m_timeElapsed;			// 시간 -> Seconds
	INT64 m_currentTime;			// 현재 시간
	INT64 m_lastTime;				// 이전 시간
	INT64 m_perfFreq;				// 주파수 . QueryPerformanceFrequency .

	FLOAT m_frameTime;				// 초당 프레임 수
	ULONG m_sampleCount;			// 현재 시간 샘플 수

	unsigned long m_frmaeRate;		// Tick Func 호출 횟수
	unsigned long m_fpsFrameCount;	// 해당 초 동안 발생한 총 프레임 수. -> m_frameRate 에 저장.
	FLOAT m_fpsTimeElapsed;			// 처리 중인 현재 초가 시작된 후 경과한 시간.
};

#endif //__CHAE_Timer_H