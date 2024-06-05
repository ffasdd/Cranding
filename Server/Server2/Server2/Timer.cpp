#include "pch.h"
#include "Timer.h"
#include"Monster.h"
#include"Session.h"

extern array<Monster, MAX_NPC> Monsters; //ÀüÃ¼ NPC 
extern HANDLE _IocpHandle;

Timer::Timer()
{
	isRunning = true;
	m_timerthread = std::thread{ [&]() {TimerThread(); } };
}

Timer::~Timer()
{
	isRunning = false;
	if (m_timerthread.joinable())
		m_timerthread.join();
	 
}

void Timer::TimerThread()
{
	while (isRunning)
	{
		TIMER_EVENT ev;
		auto cur_time = std::chrono::system_clock::now();
		if (!timer_queue.empty())
		{
			if (true == timer_queue.try_pop(ev))
			{
				if (ev.wakeupTime > cur_time)
				{
					timer_queue.push(ev);
					this_thread::sleep_for(30ms); //* 
					continue;
				}
				switch (ev.eventId)
				{
			
				case EVENT_TYPE::EV_NPC_UPDATE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NPC_UPDATE;	
					PostQueuedCompletionStatus(_IocpHandle, 1,ev.roomId, &ov->_over);
					break;
					//att true 
				}
				case EVENT_TYPE::EV_NPC_INITIALIZE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NPC_INITIALIZE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}

				case EVENT_TYPE::EV_DAYTIME: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::DAYTIME_TIMER;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_NIGHT: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NIGHT_TIMER;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}

				}
			}
			continue;
		}
		else
		{
			this_thread::yield();
		}
	}
}

void Timer::InitTimerQueue(TIMER_EVENT ev)
{
	std::lock_guard<std::mutex> timer_lg{ m_TimerQueueLock };
	timer_queue.push(ev);
}


