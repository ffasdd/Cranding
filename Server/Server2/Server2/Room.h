#pragma once
#include"Session.h"

class Session;
enum class roomState : int { Free, Ingame };

class Room
{
public:
	//mutex _room_lock; // ? �ʿ��ұ�?
	// �濡 ������ ingamePlayer;
	vector<Session*> ingamePlayer; 

	roomState _state = roomState::Free;

};

