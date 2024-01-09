#pragma once
class GameObject
{
protected:

	int  _id;
	int _hp;
	int _att;

	S_STATE _state;
	XMFLOAT3 _pos;
	atomic_bool	_is_active;
public:
	int getId() { return _id; }
	int getHP() { return _hp; }
	int getAtt() { return _att; }
	S_STATE getState() { return _state; }
	void setId(int id) { _id = id; }
	void setHP(int hp) { _hp = hp; }
	void setAtt(int att) { _att = att; }
	void setState(S_STATE state) { _state = state; }
	void setPos(float x, float y, float z) { _pos.x = x; _pos.y = y; _pos.z = z; }
	mutex _s_lock;
	//mutex getStateMutex() { return _s_lock; }

};

class Player : public GameObject
{
private:
	SOCKET _socket;
	char _name[NAME_SIZE];
	unordered_set<int> _view_list;
	mutex _vl;
	int last_move_time;
	int _prev_remain;

public:
	Player()
	{

	}
	~Player()
	{

	}
	void do_recv();

	void do_send(void* pakcet);

	void send_login_info_packet(int c_id);

	void send_add_packet(int c_id);

	void send_move_packet(int c_id);

	void send_remove_packet(int c_id);
public:
	void setPrev_remain(int prev_remain) { _prev_remain = prev_remain; }
	int getPrev_remain() { return _prev_remain; }
	void setRemain(int prevremain) { _prev_remain = prevremain; }
	void setSocket(SOCKET socket) { _socket = socket; }
	SOCKET getSocket() { return _socket; }


};
