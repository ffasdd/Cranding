#pragma once
#include"Over_Exp.h"
enum class S_STATE{ST_FREE,ST_ALLOC,ST_INGAME};
class Session
{
	Over_Exp _recv_over;

public:
	mutex _s_lock;
	 S_STATE _state;
	atomic_bool	_is_active;
	int _id;
	int		_db_id;
	int hp;
	int max_hp;
	SOCKET _socket;
	XMFLOAT3	x, y;
	char	_name[NAME_SIZE];
	int		_prev_remain;
	unordered_set <int> _view_list;
	mutex	_vl;
	int		last_move_time;

	mutex	_ll;
public:
	Session()
	{
		_id = -1;
		_socket = 0;

		_name[0] = 0;
		_state = S_STATE::ST_FREE;
		_prev_remain = 0;
		max_hp = 100;
		hp = max_hp;
	}
	~Session() {}

	void do_recv();
	void do_send(void* packet);

};

