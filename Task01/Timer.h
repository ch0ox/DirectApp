#pragma once
#include <windows.h>
#include <mmsystem.h>

#ifndef __CHAE_Timer_H
#define __CHAE_Timer_H

#define MAX_COUNT 50

class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	VOID			Tick(float fLockFPS = 0.0f);
	FLOAT			GetTimeElapsed() const;
	ULONG			GetFrameRate(LPTSTR lpszString = NULL) const;


private:
	BOOL m_perfHardware = TRUE;					// TRUE : QueryPerformanceCounter .  FALSE : timeGetTime .
	FLOAT m_timeScale = 0;						// Query Func -> Seconds
	FLOAT m_timeElapsed = 0;					// �ð� -> Seconds
	INT64 m_currentTime = 0;					// ���� �ð�
	INT64 m_lastTime = 0;						// ���� �ð�
	INT64 m_perfFreq = 0;						// ���ļ� . QueryPerformanceFrequency .

	FLOAT m_frameTime[MAX_COUNT] = { 0, };		// �ʴ� ������ ��
	ULONG m_sampleCount = 0;					// ���� �ð� ���� ��

	unsigned long m_frameRate = 0;				// Tick Func ȣ�� Ƚ��
	unsigned long m_fpsFrameCount = 0;			// �ش� �� ���� �߻��� �� ������ ��. -> m_frameRate �� ����.
	FLOAT m_fpsTimeElapsed = 0;					// ó�� ���� ���� �ʰ� ���۵� �� ����� �ð�.
};

#endif //__CHAE_Timer_H