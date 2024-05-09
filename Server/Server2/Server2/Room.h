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
	//	// 다른 Room 객체의 상태를 복사합니다.
	//	_state = other._state;
	//	// 다른 Room 객체의 ingamePlayer를 복사합니다.
	//	for (Session* player : other.ingamePlayer)
	//	{
	//		// 새로운 Session 객체를 생성하고, 기존의 Session 객체와 동일한 상태를 가지도록 복사합니다.
	//		Session* newPlayer = new Session(*player);

	//		// 복사한 Session 객체를 현재 Room 객체의 ingamePlayer에 추가합니다.
	//		ingamePlayer.push_back(newPlayer);
	//	}

	//};
	//Room& operator=(const Room& other) = delete;
	//mutex _room_lock; // ? 필요할까?
	// 방에 들어오면 ingamePlayer;
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

