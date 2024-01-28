#pragma once
#include"Over_Exp.h"
enum class STATE : uint8
{
	Free,
	Alloc,
	Ingame
};
class Session
{
	Over_Exp recv_over;
public:
	Mutex s_lock;
	STATE _state;
	atomic_bool _is_active;
	int _id;
	int hp;
	int max_hp;
	SOCKET _socket;
	XMFLOAT3 _pos;
	char _name[100];
	int _prevremain;
	unordered_set<int>_view_list;
	Mutex _vl;
public:
	Session()
	{
		_id = -1;
		_socket = 0;
		XMFLOAT3 _pos{ 0.0f,0.0f,0.0f };
		_name[0] = 0;
		_state = STATE::Free;
		_prevremain = 0;
		max_hp = 0;
		hp = max_hp;
	}
	~Session() {}
	void do_recv();
	void do_send(void* packet);

};

