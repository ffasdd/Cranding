#pragma once
#include "Session.h"
#include "Monster.h"
class Session;

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

	int startcnt = 0;
	clock_t start_time;
	clock_t end_time;

	// Night Monster
	array<Monster, 10> NightMonster;
	// Fire Monster
	array<Monster, 30> FireMonster;
	// Ice Monster
	array<Monster, 30> IceMonster;
	// Nature Monster 
	array<Monster, 30> NatureMonster;


public:
	void MoveKnightMonster();
	void SendAddMonster(int npc_id,int _id);

};

