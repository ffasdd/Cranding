#include "pch.h"
#include "Session.h"
array<Session, MAX_USER> clients;
Session::Session()
{
	_id = -1;
	_socket = 0;
	_pos.x = 0.0f;
	_pos.y = 0.0f;
	_pos.z = 0.0f;
	_name[0] = 0;
	_state = STATE::Free;
	_prevremain = 0;
	_maxhp = 100;
	_hp = _maxhp;
}


