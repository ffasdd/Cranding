#pragma once
#include "Monster.h"
#include"Session.h"
class Session;
class Monster;

class IceBossMonster;
class FireBossMonster;
class NatureBossMonster;

enum class roomState : int { Free, Ingame };
class Room
{
public:
	Room() = default;
	
public:
	vector<Session*> ingamePlayer;
	roomState _state = roomState::Free;
	mutex r_l;
	bool fullcheck = false;

	int _id = -1;
	int readycnt = 0;
	std::chrono::system_clock::time_point start_time;



	//array < Monster*, 10 >_NightMonsters; // 이렇게 바꿔야하나? 

	// Night Monster
	array<Monster, 30> NightMonster;

	array<Monster, 10> IceMonster;
	array<Monster, 10> FireMonster;
	array<Monster, 10 > NatureMonster;


	FireBossMonster FireBoss;
	IceBossMonster IceBoss;
	NatureBossMonster NatureBoss;


	Spaceship _spaceship;

	//// Fire Monster
	//array<Monster, 30> FireMonster;
	//// Ice Monster
	//array<Monster, 30> IceMonster;
	//// Nature Monster 
	//array<Monster, 30> NatureMonster;

	bool iceupdate = false;
	bool fireupdate = false;
	bool natureupdate = false;

	bool icebossskill = false;
	bool firebossskill = false;
	bool naturebossskill = false;

public:
	void SendMoveNightMonster(int npc_id);
	void SendAddMonster(int npc_id, int _id);
	//void SendRemoveMonster(int npc_id);
	void UpdateNpc();
	void DeleteNpc();

	void IceUpdateNpc();
	void FireUpdateNpc();
	void NatureUpdateNpc();

	void IceBossUpdate();
	void FireBossUpdate();
	void NatureBossUpdate();
	

	void DayTimeSend();
	void NightSend();

	void BossMonsterInitialziedMonster();

	void IceNpcInitialized();
	void FireNpcInitialized();
	void NatureNpcInitialized();

	void NightMonsterCollide(Monster& _monster);

	void IceMonsterCollide(Monster& _monster);
	void FireMonsterCollide(Monster& _monster);
	void NatureMontserCollide(Monster& _monster);

};

