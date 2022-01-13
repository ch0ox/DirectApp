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


private:
	BOOL m_perfHardware = TRUE;					// TRUE : QueryPerformanceCounter .  FALSE : timeGetTime .
	FLOAT m_timeScale = 0;						// Query Func -> Seconds
	FLOAT m_timeElapsed = 0;					// �ð� -> Seconds
	__int64 m_currentTime = 0;					// ���� �ð�
	__int64 m_lastTime = 0;						// ���� �ð�
	__int64 m_perfFreq = 0;						// ���ļ� . QueryPerformanceFrequency .

	FLOAT m_frameTime[MAX_COUNT] = { 0, };		// �ʴ� ������ ��
	ULONG m_sampleCount = 0;					// ���� �ð� ���� ��

	ULONG m_frameRate = 0;						// Tick Func ȣ�� Ƚ��
	ULONG m_fpsFrameCount = 0;					// �ش� �� ���� �߻��� �� ������ ��. -> m_frameRate �� ����.
	FLOAT m_fpsTimeElapsed = 0;					// ó�� ���� ���� �ʰ� ���۵� �� ����� �ð�.

	UINT m_font;
};

#endif //__CHAE_Timer_H