#pragma once
#include "Over_Exp.h"
#include "Room.h"
#include"protocol.h"

enum class STATE: unsigned int {Free, Alloc , Ingame};

class Session
{
	Over_Exp _recv_over;

public:
	int characterType; // 0. sword , 1. gun
	bool isReady = false;
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

	int room_id;

	animateState animationstate;
	animateState prevanimationstate;

	mutex _v_lock;
	unordered_set<int> _view_list;



public:
	Session();
	
	// 복사 생성자 정의
	Session(const Session& other)
		: _recv_over(other._recv_over),
		characterType(other.characterType),
		isReady(other.isReady),
		_s_lock(), // mutex는 복사할 필요 없음
		_state(other._state),
		_is_active(other._is_active.load()), // atomic 변수 복사
		_id(other._id),
		_hp(other._hp),
		_maxhp(other._maxhp),
		_socket(other._socket),
		_pos(other._pos),
		_look(other._look),
		_right(other._right),
		_up(other._up),
		_prevremain(other._prevremain),
		animationstate(other.animationstate),
		prevanimationstate(other.prevanimationstate),
		_v_lock(), // mutex는 복사할 필요 없음
		_view_list(other._view_list) { // unordered_set 복사
		memcpy(_name, other._name, sizeof(_name)); // 문자열 복사
	}

	// 할당 연산자 정의
	Session& operator=(const Session& other) {
		if (this != &other) { // 자기 자신에게 대입되는 경우를 방지하기 위한 조건문
			_recv_over = other._recv_over;
			characterType = other.characterType;
			isReady = other.isReady;
			_state = other._state;
			_is_active.store(other._is_active.load());
			_id = other._id;
			_hp = other._hp;
			_maxhp = other._maxhp;
			_socket = other._socket;
			_pos = other._pos;
			_look = other._look;
			_right = other._right;
			_up = other._up;
			memcpy(_name, other._name, sizeof(_name)); // 문자열 복사
			_prevremain = other._prevremain;
			animationstate = other.animationstate;
			prevanimationstate = other.prevanimationstate;
			// _s_lock, _v_lock은 mutex이므로 복사 불가능하며, 복사가 필요한 경우 직접 처리해야 함
			// _view_list은 unordered_set이므로 복사 불가능하며, 복사가 필요한 경우 직접 처리해야 함
		}
		return *this;
	}

	Session(Session&& other) noexcept : _recv_over(std::move(other._recv_over)), characterType(other.characterType),
		isReady(other.isReady), _state(other._state),
		_is_active(other._is_active.load()), _id(other._id), _hp(other._hp),
		_maxhp(other._maxhp), _socket(other._socket), _pos(std::move(other._pos)),
		_look(std::move(other._look)), _right(std::move(other._right)),
		_up(std::move(other._up)), _prevremain(other._prevremain),
		animationstate(other.animationstate),
		prevanimationstate(other.prevanimationstate),
		_view_list(std::move(other._view_list)) {
		memcpy(_name, other._name, sizeof(_name));
		// 이동 후 다른 객체의 상태를 초기화
		other.characterType = 0;
		other.isReady = false;
		other._state = STATE::Free;
		other._is_active = false;
		other._id = 0;
		other._hp = 0;
		other._maxhp = 0;
		other._socket = INVALID_SOCKET;
		other._pos = { 0.0f, 0.0f, 0.0f };
		other._look = { 0.0f, 0.0f, 0.0f };
		other._right = { 0.0f, 0.0f, 0.0f };
		other._up = { 0.0f, 0.0f, 0.0f };
		memset(other._name, 0, sizeof(other._name));
		other._prevremain = 0;
		other.animationstate = animateState::FREE;
		other.prevanimationstate = animateState::FREE;
		other._view_list.clear();
	}

	// 이동 할당 연산자
	Session& operator=(Session&& other) noexcept {
		if (this != &other) {
			_recv_over = std::move(other._recv_over);
			characterType = other.characterType;
			isReady = other.isReady;
			_state = other._state;
			_is_active.store(other._is_active.load());
			_id = other._id;
			_hp = other._hp;
			_maxhp = other._maxhp;
			_socket = other._socket;
			_pos = std::move(other._pos);
			_look = std::move(other._look);
			_right = std::move(other._right);
			_up = std::move(other._up);
			memcpy(_name, other._name, sizeof(_name));
			_prevremain = other._prevremain;
			animationstate = other.animationstate;
			prevanimationstate = other.prevanimationstate;
			_view_list = std::move(other._view_list);

			// 이동 후 다른 객체의 상태를 초기화
			other.characterType = 0;
			other.isReady = false;
			other._state = STATE::Free;
			other._is_active = false;
			other._id = 0;
			other._hp = 0;
			other._maxhp = 0;
			other._socket = INVALID_SOCKET;
			other._pos = { 0.0f, 0.0f, 0.0f };
			other._look = { 0.0f, 0.0f, 0.0f };
			other._right = { 0.0f, 0.0f, 0.0f };
			other._up = { 0.0f, 0.0f, 0.0f };
			memset(other._name, 0, sizeof(other._name));
			other._prevremain = 0;
			other.animationstate = animateState::FREE;
			other.prevanimationstate = animateState::FREE;
			other._view_list.clear();
		}
		return *this;
	}

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

	void send_game_start(int r_id);

	void send_add_info_packet(int client_id);

	void send_move_packet(int client_id, XMFLOAT3 _pos);

	void send_remove_packet(int client_id);

	void send_rotate_packet(int client_id, XMFLOAT3 _look, XMFLOAT3 _right, XMFLOAT3 _up);

	void send_test_packet(int client_id);

	void send_change_animate_packet(int client_id, animateState animate, animateState prevanimate);

};
extern array<Session, MAX_USER> clients;
