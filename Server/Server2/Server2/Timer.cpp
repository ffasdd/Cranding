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
				//case EVENT_TYPE::EV_WAKE_UP: {
				//	Over_Exp* ov = new Over_Exp;
				//	ov->_comptype = COMP_TYPE::NPC_WAKE_UP;
				//	ov->room_id = ev.roomId;
				//	//PostQueuedCompletionStatus(_IocpHandle, 1, ev.targetId, &ov->_over);

				//}

				//case EVENT_TYPE::EV_MOVE: {
				//	Over_Exp* ov = new Over_Exp;
				//	ov->_comptype = COMP_TYPE::NPC_MOVE;
				//	ov->room_id = ev.roomId;
				//	//PostQueuedCompletionStatus(_IocpHandle, 1, ev.targetId, &ov->_over);

				//	break;
				//}
				//case EVENT_TYPE::EV_ATTACK: {
				//	Over_Exp* ov = new Over_Exp;
				//	ov->_comptype = COMP_TYPE::NPC_ATTACK;

				//	break;
				//}
				case EVENT_TYPE::EV_NPC_UPDATE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NPC_UPDATE;	
					PostQueuedCompletionStatus(_IocpHandle, 1,ev.roomId, &ov->_over);
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


