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
	FLOAT m_timeElapsed;			// �ð� -> Seconds
	INT64 m_currentTime;			// ���� �ð�
	INT64 m_lastTime;				// ���� �ð�
	INT64 m_perfFreq;				// ���ļ� . QueryPerformanceFrequency .

	FLOAT m_frameTime;				// �ʴ� ������ ��
	ULONG m_sampleCount;			// ���� �ð� ���� ��

	unsigned long m_frmaeRate;		// Tick Func ȣ�� Ƚ��
	unsigned long m_fpsFrameCount;	// �ش� �� ���� �߻��� �� ������ ��. -> m_frameRate �� ����.
	FLOAT m_fpsTimeElapsed;			// ó�� ���� ���� �ʰ� ���۵� �� ����� �ð�.
};

#endif //__CHAE_Timer_H