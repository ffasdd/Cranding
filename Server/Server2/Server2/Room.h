#pragma once
#include "Session.h"
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

};

