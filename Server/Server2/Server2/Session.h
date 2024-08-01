#pragma once
#include<iostream>
#include "Over_Exp.h"
#include "Room.h"
#include"protocol.h"
enum class STATE: unsigned int {Free, Alloc , Ingame ,Start};
enum class INGAMESTATE : unsigned char { Ingame, Free };

class Session
{
	Over_Exp _recv_over;

public:
	int characterType; // 0. sword , 1. gun
	bool isReady = false;
	bool isDead = false;
	mutex _s_lock;
	STATE _state;
	INGAMESTATE _i_state;
	atomic_bool _is_active;
	int _id;
	int _hp;
	int _maxhp;
	int _attpow;
	int _speed;

	int _fireMonstercnt = 0;
	int _natureMonstercnt= 0;
	int _iceMontsercnt = 0;

	bool _isAttack = false;
	bool _isDamaged = false;
	SOCKET _socket;
	XMFLOAT3 _pos;
	XMFLOAT3 _look;
	XMFLOAT3 _right;
	XMFLOAT3 _up;


	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	char _name[100];
	int _prevremain;

	int room_id;
	int _stage;

	float distance = -1.0f;
	
	animateState animationstate;
	animateState prevanimationstate;

	float                           m_fBoundingSize{ 15.0f };
	BoundingSphere					m_SPBB = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), m_fBoundingSize);

	mutex _v_lock;
	
	mutex _p_lock;
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
		std::cout << "session 복사생성자 " << std::endl;
	}

	// 할당 연산자 정의
	Session& operator=(const Session& other) {
		if (this != &other) { // 자기 자신에게 대입되는 경우를 방지하기 위한 조건문
			std::cout << "session 할당 연산자 " << std::endl;
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
			_view_list = other._view_list;
		}
		return *this;
	}

	Session(Session&& other) noexcept : _recv_over(std::move(other._recv_over)), characterType(other.characterType),
		isReady(other.isReady), _state(other._state),
		_is_active(other._is_active.load()), _id(other._id), _hp(other._hp),
		_maxhp(other._maxhp), _socket(other._socket), _pos(other._pos),
		_look(other._look), _right(other._right),
		_up(other._up), _prevremain(other._prevremain),
		animationstate(other.animationstate),
		prevanimationstate(other.prevanimationstate),
		_view_list(other._view_list) {
		std::cout << " 이동 " << std::endl;
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
		std::cout << " 이동 할당 연산자 " << std::endl;
		if (this != &other) {

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
			memcpy(_name, other._name, sizeof(_name));
			_prevremain = other._prevremain;
			animationstate = other.animationstate;
			prevanimationstate = other.prevanimationstate;
			_view_list = other._view_list;

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

	friend bool operator==(const Session& lhs, const Session& rhs) {
		return lhs.characterType == rhs.characterType &&
			lhs.isReady == rhs.isReady &&
			lhs._state == rhs._state &&
			lhs._is_active == rhs._is_active &&
			lhs._id == rhs._id &&
			lhs._hp == rhs._hp &&
			lhs._maxhp == rhs._maxhp &&
			lhs._socket == rhs._socket &&
			lhs._pos.x == rhs._pos.x && lhs._pos.y == rhs._pos.y && lhs._pos.z == rhs._pos.z &&
			lhs._look.x == rhs._look.x && lhs._look.y == rhs._look.y && lhs._look.z == rhs._look.z &&
			lhs._right.x == rhs._right.x && lhs._right.y == rhs._right.y && lhs._right.z == rhs._right.z &&
			lhs._up.x == rhs._up.x && lhs._up.y == rhs._up.y && lhs._up.z == rhs._up.z &&
			strncmp(lhs._name, rhs._name, sizeof(lhs._name)) == 0 &&
			lhs._prevremain == rhs._prevremain &&
			lhs.animationstate == rhs.animationstate &&
			lhs.prevanimationstate == rhs.prevanimationstate &&
			lhs._view_list == rhs._view_list;
	}
	~Session() { closesocket(_socket); }

	void Rotate(float yaw);

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

	void send_move_packet(int client_id);

	void send_remove_packet(int client_id);

	void send_rotate_packet(int client_id);

	void send_test_packet(int client_id);

	void send_change_animate_packet(int client_id);

	void send_attack_packet(int client_id);

	void send_change_scene(int client_id, int stagenum);

	void send_ingame_start();

	void send_add_monster(int npc_id);

	void send_player_attack_mosnter(int npc_id,bool isattack, MonsterType montype);

	void send_spaceship_hp(int hp);

	void send_iceboss_skill(bool attack);

	void send_fireboss_skill(bool attack);

	void send_natureboss_skill(bool attack);

	void send_monster_attack(int npc_id,MonsterType monstertype, bool _attack);
	
	void send_player_hit(int client_id);

};
extern array<Session, MAX_USER> clients;
