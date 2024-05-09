#include "pch.h"
#include "Room.h"
#include"Timer.h"
extern HANDLE _IocpHandle;
extern Timer g_Timer;
void Room::MoveKnightMonster()
{
	//룸안에있는 몬스터들만 움직여 줘야 하니까 
	//Over_Exp* exover = new Over_Exp;
	//exover->_comptype = COMP_TYPE::NPC_MOVE;
	//PostQueuedCompletionStatus()
	EVENT_TYPE knightmonstermoveevent = EVENT_TYPE::EV_MOVE;
	g_Timer.InitTimerQueue(knightmonstermoveevent);
}

void Room::SendAddMonster(int npc_id, int _id)
{
	SC_ADD_MONSTER_PACKET p;
	p.type = SC_ADD_MONSTER;
	p.size = sizeof(SC_ADD_MONSTER_PACKET);
	p.id = npc_id;
	p.pos = XMFLOAT3(NightMonster[npc_id]._pos);
	p.look = XMFLOAT3(0.f, 0.f, 1.0f);
	p.up = XMFLOAT3(0.f, 1.f, 0.0f);
	p.right = XMFLOAT3(1.f, 0.f, 0.0f);

	ingamePlayer[_id]->do_send(&p);
}
