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
	animationstate = animateState::SWORD_IDLE;
	prevanimationstate = animateState::SWORD_IDLE;
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
	_view_list.clear();
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
	p.stage_num = 1;
	p.att = clients[_id]._attpow;
	p.speed = clients[_id]._speed;

	p.a_state = animateState::SWORD_IDLE;
	p.prev_state = animateState::SWORD_IDLE;


	do_send(&p);
}

void Session::send_add_info_packet(Session* client)
{
	SC_ADD_OBJECT_PACKET p;
	p.id = client->_id;
	p.size = sizeof(SC_ADD_OBJECT_PACKET);
	p.type = SC_ADD_OBJECT;
	p.pos = client->_pos;
	p.hp = client->_hp;
	p.Max_hp = client->_maxhp;
	p.look = client->_look;
	p.right = client->_right;
	p.up = client->_up;
	p.charactertype = client->characterType;
	p.stage_num = client->_stage;
	p.att = client->_attpow;
	p.speed = client->_speed;
	p.roomid = client->room_id;
	if (clients[_id].characterType == 0)
	{
		p.a_state = animateState::SWORD_IDLE;
		p.prev_state = animateState::SWORD_IDLE;
	}
	strcpy_s(p.name, client->_name);

	_v_lock.lock();
	_view_list.insert(client->_id);
	_v_lock.unlock();
	do_send(&p);
}

void Session::send_move_packet(Session* client)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = client->_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.pos = client->_pos;
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

void Session::send_rotate_packet(Session* client, float packetyaw)
{
	SC_ROTATE_OBJECT_PACKET p;
	p.id = client->_id;
	p.size = sizeof(SC_ROTATE_OBJECT_PACKET);
	p.type = SC_ROTATE_OBJECT;
	p.yaw = packetyaw;
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
	p.pos = clients[client_id]._pos;
	p.look = clients[client_id]._look;
	p.right = clients[client_id]._right;
	p.up = clients[client_id]._up;
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

void Session::send_player_attack_mosnter(int npc_id, bool isattack, MonsterType montype)
{
	SC_MONSTER_DIE_PACKET p;
	p.size = sizeof(SC_MONSTER_DIE_PACKET);
	p.type = SC_MONSTER_DIE;
	p.npc_id = npc_id;
	p._isattacked = isattack;
	p._montype = montype;
	do_send(&p);
}

void Session::send_spaceship_hp(int hp)
{
	SC_SPACESHIP_PACKET p;
	p.size = sizeof(SC_SPACESHIP_PACKET);
	p.type = SC_SPACESHIP_UPDATE;
	p.hp = hp;
	do_send(&p);
}

void Session::send_iceboss_skill(bool attack)
{
	SC_ICEBOSS_SKILL_PACKET p;
	p.size = sizeof(SC_ICEBOSS_SKILL_PACKET);
	p.type = SC_ICEBOSS_SKILL;
	p._isattacked = attack;
	do_send(&p);
}

void Session::send_fireboss_skill(bool attack)
{
	SC_FIREBOSS_SKILL_PACKET p;
	p.size = sizeof(SC_FIREBOSS_SKILL_PACKET);
	p.type = SC_FIREBOSS_SKILL;
	p._isattacked = attack;
	do_send(&p);
}

void Session::send_natureboss_skill(bool attack)
{
	SC_NATUREBOSS_SKILL_PACKET p;
	p.size = sizeof(SC_NATUREBOSS_SKILL_PACKET);
	p.type = SC_NATUREBOSS_SKILL;
	p._isattacked = attack;
	do_send(&p);
}

void Session::send_monster_attack(int npc_id, MonsterType monstertype, bool _attack)
{
	SC_MONSTER_ATTACK_PACKET p;
	p.size = sizeof(SC_MONSTER_ATTACK_PACKET);
	p.type = SC_MONSTER_ATTACK;
	p.monstertype = monstertype;
	p.id = npc_id;
	p.is_attack = _attack;
	do_send(&p);
}

void Session::send_player_hit(int client_id)
{
	SC_PLAYER_HIT_PACKET p;
	p.size = sizeof(SC_PLAYER_HIT_PACKET);
	p.type = SC_PLAYER_HIT;
	p.id = client_id;
	p.isdamaged = clients[client_id]._isDamaged;
	do_send(&p);
}

void Session::send_get_item(int item_num)
{
	SC_GET_ITEM_PACKET p;
	p.size = sizeof(SC_GET_ITEM_PACKET);
	p.type = SC_GET_ITEM;
	p.item_num = item_num;
	do_send(&p);
}

void Session::Rotate(float yaw)
{
	if (yaw != 0.0f) {
		m_fYaw += yaw;
		if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
		if (m_fYaw < 0.0f) m_fYaw += 360.0f;

	
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&_up), XMConvertToRadians(yaw));
		_look = Vector3::TransformNormal(_look, xmmtxRotate);
		_right = Vector3::TransformNormal(_right, xmmtxRotate);

		_look = Vector3::Normalize(_look);
		_right = Vector3::CrossProduct(_up, _look, true);
		_up = Vector3::CrossProduct(_look, _right, true);
	}
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




