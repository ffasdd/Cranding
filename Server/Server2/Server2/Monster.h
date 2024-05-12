#pragma once
class Monster
{
	//Monster();


public:

	XMFLOAT3 _pos;
	XMFLOAT3 _right;
	XMFLOAT3 _up;
	XMFLOAT3 _look;
	XMFLOAT3 _velocity;
	float _speed = 1.0f;
	int _hp;
	int _att;

	int _id;
	bool _is_active = false;
	bool _is_alive = false;

	BoundingOrientedBox _ob;

public:

	void Move();
	void Remove();
	void send_add_info_packet(int npc_id);
	void send_move_pakcet(int npc_id);
	void send_remove_packet(int npc_id);
	void send_attack_packet(int npc_id);
	void send_rotate_packet(int npc_id);
	void send_change_animate_packet(int npc_id);
};
extern array<Monster, MAX_NPC> Monsters;
