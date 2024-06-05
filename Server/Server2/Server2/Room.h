#pragma once
#include "Monster.h"
#include"Session.h"
class Session;
class Monster;
enum class roomState : int { Free, Ingame };
class Room
{
public:
	Room() = default;
	//Room(const Room& other) {
	//	// �ٸ� Room ��ü�� ���¸� �����մϴ�.
	//	_state = other._state;
	//	// �ٸ� Room ��ü�� ingamePlayer�� �����մϴ�.
	//	for (Session* player : other.ingamePlayer)
	//	{
	//		// ���ο� Session ��ü�� �����ϰ�, ������ Session ��ü�� ������ ���¸� �������� �����մϴ�.
	//		Session* newPlayer = new Session(*player);

	//		// ������ Session ��ü�� ���� Room ��ü�� ingamePlayer�� �߰��մϴ�.
	//		ingamePlayer.push_back(newPlayer);
	//	}

	//};
	//Room& operator=(const Room& other) = delete;
	//mutex _room_lock; // ? �ʿ��ұ�?
	// �濡 ������ ingamePlayer;
public:
	vector<Session*> ingamePlayer;
	roomState _state = roomState::Free;
	mutex r_l;
	bool fullcheck = false;

	int _id = -1;
	int readycnt = 0;
	std::chrono::system_clock::time_point start_time;
	

	//array < Monster*, 10 >_NightMonsters; // �̷��� �ٲ���ϳ�? 

	// Night Monster
	array<Monster, 10> NightMonster;
	//// Fire Monster
	//array<Monster, 30> FireMonster;
	//// Ice Monster
	//array<Monster, 30> IceMonster;
	//// Nature Monster 
	//array<Monster, 30> NatureMonster;


public:
	void SendMoveNightMonster(int npc_id);
	void SendAddMonster(int npc_id,int _id);
	//void SendRemoveMonster(int npc_id);
	void UpdateNpc();
	void DeleteNpc();

	void DayTimeSend();
	void NightSend();
};

