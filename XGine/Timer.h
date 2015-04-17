#pragma once
#include "XGine.h"

typedef struct XGINE_API Timer
{
public:
	i32 init()
	{
		if (!QueryPerformanceFrequency(&m_ticsPerSecond ))
			return 0;
		else
		{			
			QueryPerformanceCounter(&m_startTime);
			return 1;
		}
	}

	inline void  update()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		m_lastInterval = ((float)currentTime.QuadPart - (float)m_startTime.QuadPart) / (float)m_ticsPerSecond.QuadPart * 1000;
		m_startTime = currentTime;
	}

	inline float Get() { return m_lastInterval; }
	inline float GetCurrent()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return ((float)currentTime.QuadPart - (float)m_startTime.QuadPart) / (float)m_ticsPerSecond.QuadPart * 1000;
	}

protected:
	LARGE_INTEGER m_ticsPerSecond;
	LARGE_INTEGER m_startTime;
	float		  m_lastInterval;
}*Timer_ptr;