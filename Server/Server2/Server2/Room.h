#pragma once
#include"Session.h"

class Session;
enum class roomState : int { Free, Ingame };

class Room
{
public:
	//mutex _room_lock; // ? 필요할까?
	// 방에 들어오면 ingamePlayer;
	vector<Session*> ingamePlayer; 

	roomState _state = roomState::Free;

};

