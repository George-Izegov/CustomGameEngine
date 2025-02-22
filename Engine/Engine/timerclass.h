////////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <windows.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TimerClass
////////////////////////////////////////////////////////////////////////////////

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetDeltaTime();
	float GetTotalTime() { return m_totalTime; }

private:
	INT64 m_frequency;
	float m_ticksPerSec;
	INT64 m_startTime;
	float m_frameTime;
	float m_totalTime;
};
#endif