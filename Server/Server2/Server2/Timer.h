#pragma once
#include"pch.h"
enum class EVENT_TYPE : int  {
	EV_MOVE,
	EV_ATTACK,
};

class Timer
{
public:
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	EVENT_TYPE event_id;
	int target_id;
	constexpr bool operator < (const Timer& L)const
	{
		return (wakeup_time > L.wakeup_time);
	}
};
extern concurrency::concurrent_priority_queue<Timer> timer_queue;
