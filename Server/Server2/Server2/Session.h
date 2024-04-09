#pragma once
#include "Over_Exp.h"
#include"protocol.h"

enum class STATE: unsigned int {Free, Alloc , Ingame};

class Session
{
	Over_Exp _recv_over;

public:
	int characterType; // 0. sword , 1. gun

	mutex _s_lock;
	STATE _state;
	atomic_bool _is_active;
	int _id;
	int _hp;
	int _maxhp;
	SOCKET _socket;
	XMFLOAT3 _pos;
	XMFLOAT3 _look;
	XMFLOAT3 _right;
	XMFLOAT3 _up;
	char _name[100];
	int _prevremain;

	animateState animationstate;
	animateState prevanimationstate;

	mutex _v_lock;
	unordered_set<int> _view_list;



public:
	Session();
	~Session() { closesocket(_socket); }

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsaBuf.len = BUF_SIZE - _prevremain;
		_recv_over._wsaBuf.buf = _recv_over._sendbuf + +_prevremain;
		WSARecv(_socket, &_recv_over._wsaBuf, 1, 0, &recv_flag,
			&_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		Over_Exp* sdata = new Over_Exp{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsaBuf, 1, 0, 0, &sdata->_over, 0);
	}

	void send_login_info_packet();

	void send_add_info_packet(int client_id);

	void send_move_packet(int client_id);

	void send_remove_packet(int client_id);

	void send_rotate_packet(int client_id);

	void send_test_packet(int client_id);

	void send_change_animate_packet(int client_id);
};
extern array<Session, MAX_USER> clients;
