#pragma once
#include"pch.h"
#include"Over_Exp.h"


class Timer
{
public:
	Timer();
	~Timer();

private:
	bool isRunning = false;
	thread m_timerthread;
	concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
	std::mutex m_TimerQueueLock;
public:
	void TimerThread();
	void InitTimerQueue(TIMER_EVENT ev);
	
};
 
