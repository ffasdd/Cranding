#include "pch.h"
#include "Room.h"
#include"Timer.h"
extern HANDLE _IocpHandle;
extern Timer g_Timer;
void Room::SendMoveNightMonster(int npcid)
{
	r_l.lock();
	NightMonster[npcid].Move();
	r_l.unlock();
	SC_MOVE_MONSTER_PACKET p;
	p.type = SC_MOVE_MONSTER;
	p.size = sizeof(SC_MOVE_MONSTER_PACKET);
	p.pos = NightMonster[npcid]._pos;
	p.id = npcid;
	for (auto& pl : ingamePlayer)
	{
		pl->do_send(&p);
	}
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

void Room::UpdateNpc()
{
	NightMonsters sendMonsterupdatePacket;
	sendMonsterupdatePacket.size = sizeof(NightMonsters);
	sendMonsterupdatePacket.type = SC_MONSTER_UPDATE;
	int idx = 0;
	// ÀüÃ¼ NPC UPDATE 
	for (auto& npc : NightMonster)
	{
		if (npc._is_alive == false)
		{
			npc.Remove();
			continue;
		}
		npc.Move();
		sendMonsterupdatePacket._monster[idx]._id = idx;
		sendMonsterupdatePacket._monster[idx]._pos = npc._pos;
		idx++;
	}
	for (auto& pl : ingamePlayer)
	{
		pl->do_send(&sendMonsterupdatePacket);
	}
}
