#include "pch.h"
#include "Session.h"

array<Session, MAX_USER> clients;
Session::Session()
{
	_id = -1;
	_socket = 0;
	_pos = { 0.0f,0.0f,0.0f };
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
	p.pos = { 0.f,0.f,0.f };
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
	p.pos = clients[client_id]._pos;
	p.hp = clients[client_id]._hp;
	p.Max_hp = clients[client_id]._maxhp;
	strcpy_s(p.name, clients[client_id]._name);
	do_send(&p);
}

void Session::send_move_packet()
{
}


