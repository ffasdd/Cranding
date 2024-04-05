#pragma once
#include"Session.h"

class Room
{
public:
	map<Session, int> _lobbyPlayer;
	map<Session, int> _ingamePlayer;

};

