#pragma once
#include"Session.h"

class Room
{
public:
	map<Session, int> _lobbyPlayer;
	map<Session, int> _ingamePlayer;

	//mutex _room_lock; // ? 필요할까?
	
	Session* _clientinfo;

	void Init();
	void Run();
};

