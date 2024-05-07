#include "pch.h"
#include "Timer.h"

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
		EVENT_TYPE ev;
		auto cur_time = std::chrono::system_clock::now();
		if (!timer_queue.empty())
		{
			if (true == timer_queue.try_pop(ev))
			{
				if (wakeup_time > cur_time) 
				{
					timer_queue.push(ev);

					this_thread::sleep_for(1s);
					continue;
				}
				switch (ev)
				{
				case EVENT_TYPE::EV_MOVE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NPC_MOVE;


					break;
				}
				case EVENT_TYPE::EV_ATTACK: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NPC_ATTACK;
					PostQueuedCompletionStatus(_IocpHandle, 1, target_id, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_FIND: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::FIND_PLAYER;
					PostQueuedCompletionStatus(_IocpHandle, 1, target_id, &ov->_over);
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

void Timer::InitTimerQueue(EVENT_TYPE ev)
{
	std::lock_guard<std::mutex> timer_lg{ m_TimerQueueLock };
	timer_queue.push(ev);
}

void Timer::WakeupNpc(int npc_id, int waker)
{
}
