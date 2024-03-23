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
	p.pos = clients[_id]._pos;
	p.max_hp = _maxhp;
	p.hp = _hp;
	p.look = { 0.0f,0.0f,1.0f };
	p.right = { 1.0f,0.0f,0.0f };
	p.up = { 0.0f,1.0f,0.0f };
	p.a_state = IDLE;
	do_send(&p);
}

void Session::send_add_info_packet(int client_id)
{
	SC_ADD_OBJECT_PACKET p;
	p.id = client_id;
	p.size = sizeof(SC_ADD_OBJECT_PACKET);
	p.type = SC_ADD_OBJECT;
	p.pos = clients[client_id]._pos;
	p.hp = clients[client_id]._hp;
	p.Max_hp = clients[client_id]._maxhp;
	p.look = clients[client_id]._look;
	p.right = clients[client_id]._right;
	p.up = clients[client_id]._up;
	strcpy_s(p.name, clients[client_id]._name);

	_v_lock.lock();
	_view_list.insert(client_id);
	_v_lock.unlock();
	do_send(&p);
}

void Session::send_move_packet(int client_id)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = client_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.pos = clients[client_id]._pos;
	p.a_state = FORWARD_MOVE;
	do_send(&p);
}



void Session::send_remove_packet(int client_id)
{
	SC_REMOVE_OBJECT_PACKET p;
	p.id = client_id;
	p.size = sizeof(SC_REMOVE_OBJECT_PACKET);
	p.type = SC_REMOVE_OBJECT;
	do_send(&p);
}

void Session::send_rotate_packet(int client_id)
{
	SC_ROTATE_OBJECT_PACKET p;
	p.id = client_id;
	p.size = sizeof(SC_ROTATE_OBJECT_PACKET);
	p.type = SC_ROTATE_OBJECT;
	p.look = clients[client_id]._look;
	p.right = clients[client_id]._right;
	p.up = clients[client_id]._up;
	do_send(&p);
}

void Session::send_test_packet(int client_id)
{
	SC_TEST_PACKET p;
	p.size = sizeof(SC_TEST_PACKET);
	p.type = SC_TEST;
	do_send(&p);
}




