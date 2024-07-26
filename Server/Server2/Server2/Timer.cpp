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
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
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
				case EVENT_TYPE::EV_ICE_NPC_UPDATE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::ICE_NPC_UPDATE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_FIRE_NPC_UPDATE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::FIRE_NPC_UPDATE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_NATURE_NPC_UPDATE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NATURE_NPC_UPDATE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_ICE_BOSS_MOVE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::ICE_BOSS_MOVE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;

				}
				case EVENT_TYPE::EV_FIRE_BOSS_MOVE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::FIRE_BOSS_MOVE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;

				}
				case EVENT_TYPE::EV_NATURE_BOSS_MOVE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NATURE_BOSS_MOVE;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_PLAYER_ATTACK_NPC: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::PLAYER_ATTACKED_NPC;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_ICE_BOSS_SKILL: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::ICE_BOSS_SKILL;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_ICE_BOSS_SKILL_CANCLE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::ICE_BOSS_CANCLE_SKILL;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_FIRE_BOSS_SKILL: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::FIRE_BOSS_SKILL;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_FIRE_BOSS_SKILL_CANCLE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::FIRE_BOSS_CANCLE_SKILL;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_NATURE_BOSS_SKILL: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NATURE_BOSS_SKILL;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
					PostQueuedCompletionStatus(_IocpHandle, 1, ev.roomId, &ov->_over);
					break;
				}
				case EVENT_TYPE::EV_NATURE_BOSS_SKILL_CANCLE: {
					Over_Exp* ov = new Over_Exp;
					ov->_comptype = COMP_TYPE::NATURE_BOSS_SKILL_CANCLE;
					ov->_ai_target_obj = ev.targetId;
					ov->_monstertype = ev.monsterType;
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


