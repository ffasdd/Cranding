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

void Session::send_login_info_packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	p.x = _pos.x;
	p.y = _pos.y;
	p.z = _pos.z;
	p.max_hp = _maxhp;
	p.hp = _hp;
	do_send(&p);
}

void Session::send_add_info_packet(int client_id)
{
	SC_ADD_OBJECT_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_ADD_OBJECT_PACKET);
	p.type = SC_ADD_OBJECT;
	p.x = clients[client_id]._pos.x;
	p.y = clients[client_id]._pos.y;
	p.z = clients[client_id]._pos.z;
	p.hp = clients[client_id]._hp;
	p.Max_hp = clients[client_id]._maxhp;
	strcpy_s(p.name, clients[client_id]._name);
	do_send(&p);
}

void Session::send_move_packet()
{
}


