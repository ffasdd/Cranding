#include "pch.h"
#include "Room.h"
//#include"protocol.h"
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

	NightMonstersUpdate sendmonsterupdatePacket[10];
	int idx = 0;

	// 전체 NPC UPDATE 
	for (auto& npc : NightMonster)
	{
		npc.ingamePlayer = ingamePlayer;
		if (npc._is_alive == false)
		{
			npc.Remove();
		}
		else
			npc.Move();

		sendmonsterupdatePacket[idx].size = sizeof(NightMonstersUpdate);
		sendmonsterupdatePacket[idx].type = SC_MONSTER_UPDATE_POS;
		sendmonsterupdatePacket[idx]._monster._id = idx;
							
		sendmonsterupdatePacket[idx]._monster._x = npc._pos.x;
		sendmonsterupdatePacket[idx]._monster._y = npc._pos.y;
		sendmonsterupdatePacket[idx]._monster._z = npc._pos.z;
							 
		sendmonsterupdatePacket[idx]._monster._lx = npc._look.x;
		sendmonsterupdatePacket[idx]._monster._ly = npc._look.y;
		sendmonsterupdatePacket[idx]._monster._lz = npc._look.z;
							  
		sendmonsterupdatePacket[idx]._monster._rx = npc._right.x;
		sendmonsterupdatePacket[idx]._monster._ry = npc._right.y;
		sendmonsterupdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}


	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendmonsterupdatePacket)
		{
			if(pl->_stage == 2) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
				pl->do_send(&packet);
		}
	}
}

void Room::DeleteNpc()
{
	NightMonstersUpdate sendMonsterDead;
	sendMonsterDead.size = sizeof(NightMonstersUpdate);
	sendMonsterDead.type = SC_MONSTER_UPDATE_POS;
	int idxx = 0;
	for (auto& npc : NightMonster)
	{

	}

}

void Room::DayTimeSend()
{
	SC_DAYTIME_PACKET p;
	p.size = sizeof(SC_DAYTIME_PACKET);
	p.type = SC_DAYTIME;
	for (auto& pl : ingamePlayer)
	{
		pl->do_send(&p);
	}
}

void Room::NightSend()
{
	SC_NIGHT_PACKET p;
	p.size = sizeof(SC_NIGHT_PACKET);
	p.type = SC_NIGHT;
	for (auto& pl : ingamePlayer)
	{
		pl->do_send(&p);
	}
}
