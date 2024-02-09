#pragma once
class Session
{
public:
	Session() {
		m_id = 0;
		m_name[0] = 0;
		m_hp = 0;
		_pos = { 0.0f,0.0f,0.f };
		_right_vec = { 0.f,0.f,0.f };
		_up_vec = { 0.f,0.f,0.f };
		_look_vec = { 0.f,0.f,0.f };
	}
public:
	short m_id;
	char m_name[20];
	int m_hp;
	XMFLOAT3 _pos;
	XMFLOAT3 _right_vec;
	XMFLOAT3 _up_vec;
	XMFLOAT3 _look_vec;

	int _state;
	
};

