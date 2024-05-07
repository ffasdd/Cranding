#pragma once
#include"pch.h"
#include"Over_Exp.h"
enum class EVENT_TYPE : int  {
	EV_MOVE,
	EV_ATTACK,
	EV_FIND
};

class Timer
{
public:
	Timer();
	~Timer();

public:
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	EVENT_TYPE event_id;
	int target_id;
	constexpr bool operator < (const Timer& L)const
	{
		return (wakeup_time > L.wakeup_time);
	}

private:
	bool isRunning = false;
	thread m_timerthread;
	concurrency::concurrent_priority_queue<EVENT_TYPE> timer_queue;
	std::mutex m_TimerQueueLock;
public:
	void TimerThread();
	void InitTimerQueue(EVENT_TYPE ev);
	void WakeupNpc(int npc_id, int waker);
};
 
