#include "pch.h"
#include "Session.h"


array<Session, MAX_USER> clients; //전체 클라이언트 
std::random_device rd;
std::default_random_engine dre;
std::uniform_real_distribution<float> m_xpos(-10, 450);
std::uniform_real_distribution<float> m_zpos(-641, -521);

std::uniform_real_distribution<float> p_xpos(-120, 674);
std::uniform_real_distribution<float> p_zpos(-312, 455);


Session::Session()
{
	characterType = -1;
	animationstate = animateState::FREE;
	prevanimationstate = animateState::FREE;
	_id = -1;
	_socket = 0;
	_pos = { 0.0f,0.0f,0.0f };
	_look = { 0.0f,0.0f,1.0f };
	_up = { 0.0f,1.0f,0.0f };
	_right = { 1.0f,0.0f,0.0f };
	_name[0] = 0;
	_state = STATE::Free;
	_prevremain = 0;
	_maxhp = 100;
	_hp = _maxhp;
	room_id = -1;
	_stage = 0;
}

void Session::send_login_info_packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	clients[_id]._pos.x = p_xpos(dre);
	clients[_id]._pos.z = p_zpos(dre);
	p.pos = clients[_id]._pos;
	p.max_hp = _maxhp;
	p.hp = _hp;
	p.look = { 0.0f,0.0f,1.0f };
	p.up = { 0.0f,1.0f,0.0f };
	p.right = { 1.0f,0.0f,0.0f };
	p.charactertype = clients[_id].characterType;
	p.room_id = clients[_id].room_id;

	if (clients[_id].characterType == 0)
	{
		p.a_state = animateState::SWORD_IDLE;
		p.prev_state = animateState::SWORD_IDLE;
	}
	else if (clients[_id].characterType == 1)
	{
		p.a_state = animateState::GUN_IDLE;
		p.prev_state = animateState::GUN_IDLE;

	}

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
	p.charactertype = clients[client_id].characterType;

	if (clients[_id].characterType == 0)
	{
		p.a_state = animateState::SWORD_IDLE;
		p.prev_state = animateState::SWORD_IDLE;
	}
	else if (clients[_id].characterType == 1)
	{
		p.a_state = animateState::GUN_IDLE;
		p.prev_state = animateState::GUN_IDLE;

	}

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
	//p.pos =_pos; // 여기서 클라이언트 포스를 보내고 있기 때문에 이동 확인 X  
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
	p.up = { 0.f,1.0f,0.f };
	do_send(&p);
}

void Session::send_test_packet(int client_id)
{
	SC_TEST_PACKET p;
	p.size = sizeof(SC_TEST_PACKET);
	p.type = SC_TEST;
	do_send(&p);
}

void Session::send_change_animate_packet(int client_id)
{
	SC_CHANGE_ANIMATION_PACKET p;
	p.size = sizeof(SC_CHANGE_ANIMATION_PACKET);
	p.id = client_id;
	p.type = SC_CHANGE_ANIMATION;
	p.a_state = clients[client_id].animationstate;
	p.prev_a_state = clients[client_id].prevanimationstate;
	do_send(&p);
}

void Session::send_attack_packet(int client_id)
{
	SC_ATTACK_PACKET p;
	p.size = sizeof(SC_ATTACK_PACKET);
	p.type = SC_ATTACK;
	p.id = client_id;
	p.isAttack = clients[client_id]._isAttack;
	do_send(&p);
}

void Session::send_change_scene(int client_id, int stagenum)
{
	SC_CHANGE_SCENE_PACKET p;
	p.size = sizeof(SC_CHANGE_SCENE_PACKET);
	p.type = SC_CHANGE_SCENE;
	p.stage = stagenum;
	p.id = client_id;
	do_send(&p);
}

void Session::send_ingame_start()
{
	SC_INGAME_START_PACKET p;
	p.type = SC_INGAME_STRAT;
	p.size = sizeof(SC_INGAME_START_PACKET);
	
	do_send(&p);
}

void Session::send_add_monster(int npc_id)
{
	SC_ADD_MONSTER_PACKET p;
	p.type = SC_ADD_MONSTER;
	p.size = sizeof(SC_ADD_MONSTER_PACKET);
	p.id = npc_id;
	p.pos = XMFLOAT3(m_xpos(dre), 10.f, m_zpos(dre));
	p.look = XMFLOAT3(0.f, 0.f, 1.0f);
	p.up = XMFLOAT3(0.f, 1.f, 0.0f);
	p.right = XMFLOAT3(1.f, 0.f, 0.0f);
	do_send(&p);
}

void Session::send_game_start(int r_id)
{
	SC_GAMESTART_PACKET p;
	p.type = SC_START_GAME;
	p.id = _id;
	p.size = sizeof(SC_GAMESTART_PACKET);
	p.roomid = r_id;
	do_send(&p);
}




