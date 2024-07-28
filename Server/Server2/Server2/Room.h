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
	int daycnt = 0;
	int _id = -1;
	int readycnt = 0;
	int deadplayercnt = 0;
	std::chrono::system_clock::time_point start_time;

	bool IceItem = false;
	bool FireItem = false;
	bool NatureItem = false;

	bool isWin = false;
	bool isLose = false;


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


};

