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

	NightMonstersUpdate sendmonsterupdatePacket[30];
	int idx = 0;

	// 전체 NPC UPDATE 
	for (auto& npc : NightMonster)
	{
		//npc.ingamePlayer = ingamePlayer;

		if (npc._is_alive == false)
		{
			npc.Remove();
		}
		else
		{
			npc.Move();
		}
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


		NightMonsterCollide(npc);

		idx++;
	}


	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendmonsterupdatePacket)
		{
			if (pl->_stage != 2) continue;
			// 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
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

void Room::IceUpdateNpc()
{
	IceMonstersUpdate sendIceMonsterUpdatePacket[10];
	int idx = 0;

	for (auto& npc : IceMonster)
	{
		if (npc._is_alive == false)
		{
			npc.Remove();
		}
		else
			npc.IceMove();

		sendIceMonsterUpdatePacket[idx].size = sizeof(IceMonstersUpdate);
		sendIceMonsterUpdatePacket[idx].type = SC_ICE_MONSTER_UPDATE;
		sendIceMonsterUpdatePacket[idx]._monster._id = idx;

		sendIceMonsterUpdatePacket[idx]._monster._x = npc._pos.x;
		sendIceMonsterUpdatePacket[idx]._monster._y = npc._pos.y;
		sendIceMonsterUpdatePacket[idx]._monster._z = npc._pos.z;

		sendIceMonsterUpdatePacket[idx]._monster._lx = npc._look.x;
		sendIceMonsterUpdatePacket[idx]._monster._ly = npc._look.y;
		sendIceMonsterUpdatePacket[idx]._monster._lz = npc._look.z;

		sendIceMonsterUpdatePacket[idx]._monster._rx = npc._right.x;
		sendIceMonsterUpdatePacket[idx]._monster._ry = npc._right.y;
		sendIceMonsterUpdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}
	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendIceMonsterUpdatePacket)
		{
			if (pl->_stage == 3) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
				pl->do_send(&packet);
		}
	}
}

void Room::FireUpdateNpc()
{
	FireMonsterUpdate sendFireMonsterUpdatePacket[10];
	int idx = 0;

	for (auto& npc : FireMonster)
	{
		if (npc._is_alive == false)
		{
			npc.Remove();
		}
		else
			npc.FireMove();

		sendFireMonsterUpdatePacket[idx].size = sizeof(FireMonsterUpdate);
		sendFireMonsterUpdatePacket[idx].type = SC_FIRE_MONSTER_UPDATE;
		sendFireMonsterUpdatePacket[idx]._monster._id = idx;

		sendFireMonsterUpdatePacket[idx]._monster._x = npc._pos.x;
		sendFireMonsterUpdatePacket[idx]._monster._y = npc._pos.y;
		sendFireMonsterUpdatePacket[idx]._monster._z = npc._pos.z;

		sendFireMonsterUpdatePacket[idx]._monster._lx = npc._look.x;
		sendFireMonsterUpdatePacket[idx]._monster._ly = npc._look.y;
		sendFireMonsterUpdatePacket[idx]._monster._lz = npc._look.z;
	
		sendFireMonsterUpdatePacket[idx]._monster._rx = npc._right.x;
		sendFireMonsterUpdatePacket[idx]._monster._ry = npc._right.y;
		sendFireMonsterUpdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}
	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendFireMonsterUpdatePacket)
		{
			if (pl->_stage == 4) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
				pl->do_send(&packet);
		}
	}
}

void Room::NatureUpdateNpc()
{
	NatureMonsterUpdate sendNatureMontserUpdatePacket[10];
	int idx = 0;

	for (auto& npc : NatureMonster)
	{
		if (npc._is_alive == false)
		{
			npc.Remove();
		}
		else
			npc.NatureMove();

		sendNatureMontserUpdatePacket[idx].size = sizeof(NatureMonsterUpdate);
		sendNatureMontserUpdatePacket[idx].type = SC_NATURE_MONSTER_UPDATE;
		sendNatureMontserUpdatePacket[idx]._monster._id = idx;
		
		sendNatureMontserUpdatePacket[idx]._monster._x = npc._pos.x;
		sendNatureMontserUpdatePacket[idx]._monster._y = npc._pos.y;
		sendNatureMontserUpdatePacket[idx]._monster._z = npc._pos.z;
		
		sendNatureMontserUpdatePacket[idx]._monster._lx = npc._look.x;
		sendNatureMontserUpdatePacket[idx]._monster._ly = npc._look.y;
		sendNatureMontserUpdatePacket[idx]._monster._lz = npc._look.z;
		
		sendNatureMontserUpdatePacket[idx]._monster._rx = npc._right.x;
		sendNatureMontserUpdatePacket[idx]._monster._ry = npc._right.y;
		sendNatureMontserUpdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}
	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendNatureMontserUpdatePacket)
		{
			if (pl->_stage == 5) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
				pl->do_send(&packet);
		}
	}
}

void Room::IceBossUpdate()
{
	BossUpdate_Ice sendIceBossUpdatePacket;
	if (IceBoss._is_alive == true)
	{
		IceBoss.Move();
	}
	else
	{
		IceBoss.Remove();
	}

	sendIceBossUpdatePacket.size = sizeof(BossUpdate_Ice);
	sendIceBossUpdatePacket.type = SC_ICE_BOSS_UPDATE;
	
	sendIceBossUpdatePacket._boss._x = IceBoss._pos.x;
	sendIceBossUpdatePacket._boss._y = IceBoss._pos.y;
	sendIceBossUpdatePacket._boss._z = IceBoss._pos.z;

	sendIceBossUpdatePacket._boss._lx = IceBoss._look.x;
	sendIceBossUpdatePacket._boss._ly = IceBoss._look.y;
	sendIceBossUpdatePacket._boss._lz = IceBoss._look.z;

	sendIceBossUpdatePacket._boss._rx = IceBoss._right.x;
	sendIceBossUpdatePacket._boss._ry = IceBoss._right.y;
	sendIceBossUpdatePacket._boss._rz = IceBoss._right.z;


	for (auto& cl : ingamePlayer)
	{
		if (cl->_stage == 3)
			cl->do_send(&sendIceBossUpdatePacket);
	}
}

void Room::FireBossUpdate()
{
	BossUpdate_Fire sendFireBossUpdatePacket;
	if (FireBoss._is_alive == true)
	{
		FireBoss.Move();
	}
	else
	{
		FireBoss.Remove();
	}

	sendFireBossUpdatePacket.size = sizeof(BossUpdate_Fire);
	sendFireBossUpdatePacket.type = SC_FIRE_BOSS_UPDATE;
	
	sendFireBossUpdatePacket._boss._x = FireBoss._pos.x;
	sendFireBossUpdatePacket._boss._y = FireBoss._pos.y;
	sendFireBossUpdatePacket._boss._z = FireBoss._pos.z;
	
	sendFireBossUpdatePacket._boss._lx = FireBoss._look.x;
	sendFireBossUpdatePacket._boss._ly = FireBoss._look.y;
	sendFireBossUpdatePacket._boss._lz = FireBoss._look.z;
	
	sendFireBossUpdatePacket._boss._rx = FireBoss._right.x;
	sendFireBossUpdatePacket._boss._ry = FireBoss._right.y;
	sendFireBossUpdatePacket._boss._rz = FireBoss._right.z;


	for (auto& cl : ingamePlayer)
	{
		if (cl->_stage == 4)
			cl->do_send(&sendFireBossUpdatePacket);
	}
}

void Room::NatureBossUpdate()
{
	BossUpdate_Nature sendNatureBossUpdatePacket;
	if (NatureBoss._is_alive == true)
	{
		NatureBoss.Move();
	}
	else
	{
		NatureBoss.Remove();
	}

	sendNatureBossUpdatePacket.size = sizeof(BossUpdate_Nature);
	sendNatureBossUpdatePacket.type = SC_NATURE_BOSS_UPDATE;

	sendNatureBossUpdatePacket._boss._x = NatureBoss._pos.x;
	sendNatureBossUpdatePacket._boss._y = NatureBoss._pos.y;
	sendNatureBossUpdatePacket._boss._z = NatureBoss._pos.z;

	sendNatureBossUpdatePacket._boss._lx = NatureBoss._look.x;
	sendNatureBossUpdatePacket._boss._ly = NatureBoss._look.y;
	sendNatureBossUpdatePacket._boss._lz = NatureBoss._look.z;

	sendNatureBossUpdatePacket._boss._rx = NatureBoss._right.x;
	sendNatureBossUpdatePacket._boss._ry = NatureBoss._right.y;
	sendNatureBossUpdatePacket._boss._rz = NatureBoss._right.z;


	for (auto& cl : ingamePlayer)
	{
		if (cl->_stage == 5)
			cl->do_send(&sendNatureBossUpdatePacket);
	}
}

void Room::DayTimeSend()
{
	SC_DAYTIME_PACKET p;
	p.size = sizeof(SC_DAYTIME_PACKET);
	p.type = SC_DAYTIME;

	for (auto& pl : ingamePlayer)
	{
		p.firecnt = pl->_fireMonstercnt;
		p.icecnt = pl->_iceMontsercnt;
		p.naturecnt = pl->_natureMonstercnt;

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

void Room::BossMonsterInitialziedMonster()
{
	FireBoss._pos = XMFLOAT3(-60.0f, 10.0f, 1327.0f);
	FireBoss._is_alive = true;
	FireBoss._stagenum = 4;
	FireBoss._m_type = MonsterType::Fire_Boss;
	FireBoss._initPos = FireBoss._pos;

	IceBoss._pos = XMFLOAT3(-12.0f, 10.0f, -220.f);
	IceBoss._is_alive = true;
	IceBoss._stagenum = 3;
	IceBoss._m_type = MonsterType::Ice_Boss;
	IceBoss._initPos = IceBoss._pos;

	NatureBoss._pos = XMFLOAT3(77.0f, 10.0f, -408.0f);
	NatureBoss._is_alive = true;
	NatureBoss._stagenum = 5;
	NatureBoss._m_type = MonsterType::Nature_Boss;
	NatureBoss._initPos = NatureBoss._pos;

	BossUpdate_Fire sendFireBossMonsterInitialziedpacket;
	BossUpdate_Ice sendIceBossMonsterInitialziedpacket;
	BossUpdate_Nature sendNatureBossMonsterInitialziedpacket;

	sendFireBossMonsterInitialziedpacket.size = sizeof(BossUpdate_Fire);
	sendFireBossMonsterInitialziedpacket.type = SC_FIRE_BOSS_UPDATE;
	sendFireBossMonsterInitialziedpacket._boss._x = FireBoss._pos.x;
	sendFireBossMonsterInitialziedpacket._boss._y = FireBoss._pos.y;
	sendFireBossMonsterInitialziedpacket._boss._z = FireBoss._pos.z;

	sendFireBossMonsterInitialziedpacket._boss._lx = FireBoss._look.x;
	sendFireBossMonsterInitialziedpacket._boss._ly = FireBoss._look.y;
	sendFireBossMonsterInitialziedpacket._boss._lz = FireBoss._look.z;
															  
	sendFireBossMonsterInitialziedpacket._boss._rx = FireBoss._right.x;
	sendFireBossMonsterInitialziedpacket._boss._ry = FireBoss._right.y;
	sendFireBossMonsterInitialziedpacket._boss._rz = FireBoss._right.z;

	sendIceBossMonsterInitialziedpacket.size = sizeof(BossUpdate_Ice);
	sendIceBossMonsterInitialziedpacket.type = SC_ICE_BOSS_UPDATE;
	sendIceBossMonsterInitialziedpacket._boss._x = IceBoss._pos.x;
	sendIceBossMonsterInitialziedpacket._boss._y = IceBoss._pos.y;
	sendIceBossMonsterInitialziedpacket._boss._z = IceBoss._pos.z;

	sendIceBossMonsterInitialziedpacket._boss._lx = IceBoss._look.x;
	sendIceBossMonsterInitialziedpacket._boss._ly = IceBoss._look.y;
	sendIceBossMonsterInitialziedpacket._boss._lz = IceBoss._look.z;

	sendIceBossMonsterInitialziedpacket._boss._rx = IceBoss._right.x;
	sendIceBossMonsterInitialziedpacket._boss._ry = IceBoss._right.y;
	sendIceBossMonsterInitialziedpacket._boss._rz = IceBoss._right.z;

	sendNatureBossMonsterInitialziedpacket.size = sizeof(BossUpdate_Nature);
	sendNatureBossMonsterInitialziedpacket.type = SC_NATURE_BOSS_UPDATE;
	sendNatureBossMonsterInitialziedpacket._boss._x = NatureBoss._pos.x;
	sendNatureBossMonsterInitialziedpacket._boss._y = NatureBoss._pos.y;
	sendNatureBossMonsterInitialziedpacket._boss._z = NatureBoss._pos.z;

	sendNatureBossMonsterInitialziedpacket._boss._lx = NatureBoss._look.x;
	sendNatureBossMonsterInitialziedpacket._boss._ly = NatureBoss._look.y;
	sendNatureBossMonsterInitialziedpacket._boss._lz = NatureBoss._look.z;

	sendNatureBossMonsterInitialziedpacket._boss._rx = NatureBoss._right.x;
	sendNatureBossMonsterInitialziedpacket._boss._ry = NatureBoss._right.y;
	sendNatureBossMonsterInitialziedpacket._boss._rz = NatureBoss._right.z;

	for (auto& pl : ingamePlayer)
	{
		pl->do_send(&sendIceBossMonsterInitialziedpacket);
		pl->do_send(&sendFireBossMonsterInitialziedpacket);
		pl->do_send(&sendNatureBossMonsterInitialziedpacket);
	}
}

void Room::IceNpcInitialized()
{
	std::random_device rd;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> xpos(-450, 700);
	std::uniform_real_distribution<float> zpos(206, 658);

	for (int i = 0; i < IceMonster.max_size(); ++i)
	{
		IceMonster[i]._id = i;
		IceMonster[i]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));
		IceMonster[i]._look = XMFLOAT3(0.f, 0.f, 1.0f);
		IceMonster[i]._up = XMFLOAT3(0.f, 1.f, 0.0f);
		IceMonster[i]._right = XMFLOAT3(1.f, 0.f, 0.0f);
		IceMonster[i]._att = 10;
		IceMonster[i]._hp = 50;
		IceMonster[i]._is_alive = true;
		IceMonster[i]._m_type = MonsterType::Ice;
		IceMonster[i]._stagenum = 3;
		IceMonster[i]._initPos = IceMonster[i]._pos;
	}

	IceMonstersUpdate sendIceMonsterUpdatePacket[10];
	int idx = 0;

	for (auto& npc : IceMonster)
	{
		sendIceMonsterUpdatePacket[idx].size = sizeof(IceMonstersUpdate);
		sendIceMonsterUpdatePacket[idx].type = SC_ICE_MONSTER_UPDATE;
		sendIceMonsterUpdatePacket[idx]._monster._id = idx;

		sendIceMonsterUpdatePacket[idx]._monster._x = npc._pos.x;
		sendIceMonsterUpdatePacket[idx]._monster._y = npc._pos.y;
		sendIceMonsterUpdatePacket[idx]._monster._z = npc._pos.z;

		sendIceMonsterUpdatePacket[idx]._monster._lx = npc._look.x;
		sendIceMonsterUpdatePacket[idx]._monster._ly = npc._look.y;
		sendIceMonsterUpdatePacket[idx]._monster._lz = npc._look.z;

		sendIceMonsterUpdatePacket[idx]._monster._rx = npc._right.x;
		sendIceMonsterUpdatePacket[idx]._monster._ry = npc._right.y;
		sendIceMonsterUpdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}

	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendIceMonsterUpdatePacket)
		{
			//if (pl->_stage == 3) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
			pl->do_send(&packet);
		}
	}
}

void Room::FireNpcInitialized()
{
	std::random_device rd;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> xpos(-50, 380);
	std::uniform_real_distribution<float> zpos(-320, 1250);


	for (int i = 0; i < FireMonster.max_size(); ++i)
	{
		FireMonster[i]._id = i;
		FireMonster[i]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));
		FireMonster[i]._look = XMFLOAT3(0.f, 0.f, 1.0f);
		FireMonster[i]._up = XMFLOAT3(0.f, 1.f, 0.0f);
		FireMonster[i]._right = XMFLOAT3(1.f, 0.f, 0.0f);
		FireMonster[i]._att = 10;
		FireMonster[i]._hp = 50;
		FireMonster[i]._is_alive = true;
		FireMonster[i]._m_type = MonsterType::Fire;
		FireMonster[i]._stagenum = 4;
		FireMonster[i]._initPos = FireMonster[i]._pos;
	}

	FireMonsterUpdate sendFireMonsterUpdatePacket[10];
	int idx = 0;

	for (auto& npc : FireMonster)
	{
		sendFireMonsterUpdatePacket[idx].size = sizeof(FireMonsterUpdate);
		sendFireMonsterUpdatePacket[idx].type = SC_FIRE_MONSTER_UPDATE;
		sendFireMonsterUpdatePacket[idx]._monster._id = idx;

		sendFireMonsterUpdatePacket[idx]._monster._x = npc._pos.x;
		sendFireMonsterUpdatePacket[idx]._monster._y = npc._pos.y;
		sendFireMonsterUpdatePacket[idx]._monster._z = npc._pos.z;
	
		sendFireMonsterUpdatePacket[idx]._monster._lx = npc._look.x;
		sendFireMonsterUpdatePacket[idx]._monster._ly = npc._look.y;
		sendFireMonsterUpdatePacket[idx]._monster._lz = npc._look.z;
	
		sendFireMonsterUpdatePacket[idx]._monster._rx = npc._right.x;
		sendFireMonsterUpdatePacket[idx]._monster._ry = npc._right.y;
		sendFireMonsterUpdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}

	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendFireMonsterUpdatePacket)
		{
			//if (pl->_stage == 3) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
			pl->do_send(&packet);
		}
	}

}

void Room::NatureNpcInitialized()
{
	std::random_device rd;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> xpos(50, 360);
	std::uniform_real_distribution<float> zpos(-700, -350);

	for (int i = 0; i < NatureMonster.max_size(); ++i)
	{
		NatureMonster[i]._id = i;
		NatureMonster[i]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));
		NatureMonster[i]._look = XMFLOAT3(0.f, 0.f, 1.0f);
		NatureMonster[i]._up = XMFLOAT3(0.f, 1.f, 0.0f);
		NatureMonster[i]._right = XMFLOAT3(1.f, 0.f, 0.0f);
		NatureMonster[i]._att = 10;
		NatureMonster[i]._hp = 50;
		NatureMonster[i]._is_alive = true;
		NatureMonster[i]._m_type = MonsterType::Nature;
		NatureMonster[i]._stagenum = 5;
		NatureMonster[i]._initPos = NatureMonster[i]._pos;
	}

	NatureMonsterUpdate sendNatureMonsterUpdatePacket[10];
	int idx = 0;

	for (auto& npc : NatureMonster)
	{
		sendNatureMonsterUpdatePacket[idx].size = sizeof(NatureMonsterUpdate);
		sendNatureMonsterUpdatePacket[idx].type = SC_NATURE_MONSTER_UPDATE;
		sendNatureMonsterUpdatePacket[idx]._monster._id = idx;
		
		sendNatureMonsterUpdatePacket[idx]._monster._x = npc._pos.x;
		sendNatureMonsterUpdatePacket[idx]._monster._y = npc._pos.y;
		sendNatureMonsterUpdatePacket[idx]._monster._z = npc._pos.z;
		
		sendNatureMonsterUpdatePacket[idx]._monster._lx = npc._look.x;
		sendNatureMonsterUpdatePacket[idx]._monster._ly = npc._look.y;
		sendNatureMonsterUpdatePacket[idx]._monster._lz = npc._look.z;
		
		sendNatureMonsterUpdatePacket[idx]._monster._rx = npc._right.x;
		sendNatureMonsterUpdatePacket[idx]._monster._ry = npc._right.y;
		sendNatureMonsterUpdatePacket[idx]._monster._rz = npc._right.z;


		idx++;
	}

	for (auto& pl : ingamePlayer)
	{
		// 한번 업데이트할때마다 10개의 패킷을 보내야되는건데 
		for (auto& packet : sendNatureMonsterUpdatePacket)
		{
			//if (pl->_stage == 3) // 클라이언트가 우주선 씬에 있을 때에만 공격하는 NPC들의 패킷을 보냄 
			pl->do_send(&packet);
		}
	}
}

void Room::NightMonsterCollide(Monster& _monster)
{
	for (auto& monster : NightMonster)
	{
		if (monster._id == _monster._id)continue;
		if (monster._is_alive == false)continue;
		XMVECTOR otherPos = XMLoadFloat3(&monster._pos);
		XMVECTOR myPos = XMLoadFloat3(&_monster._pos);

		XMVECTOR disVec = XMVectorSubtract(otherPos, myPos);
		disVec = XMVector3Normalize(disVec);

		XMVECTOR lookDir = XMLoadFloat3(&_monster._look);
		lookDir = XMVector3Normalize(lookDir);

		float dotProduct = XMVectorGetX(XMVector3Dot(lookDir, disVec));


		float angleThreshold = cosf(XMConvertToRadians(45.0f)); // 45도 이내
		if (dotProduct > angleThreshold)
		{
			if (_monster.m_SPBB.Intersects(monster.m_SPBB))
			{
				_monster._speed = 0.f;

			}
		}
	}

}


