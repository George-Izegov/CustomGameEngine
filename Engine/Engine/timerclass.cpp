///////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timerclass.h"


TimerClass::TimerClass()
{
	m_totalTime = 0.0f;
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}


bool TimerClass::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if(m_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every second.
	m_ticksPerSec = (float)(m_frequency);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}


void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerSec;

	m_totalTime += m_frameTime;

	m_startTime = currentTime;

	return;
}


float TimerClass::GetDeltaTime()
{
	return m_frameTime;
}