#pragma once
class Monster
{
	//Monster();


public:

	XMFLOAT3 _pos;
	XMFLOAT3 _right;
	XMFLOAT3 _up;
	XMFLOAT3 _look;
	int _hp;
	int _att;

	int _id;
	bool _is_active = false;

	BoundingOrientedBox _ob;

public:
	
	void send_add_info_packet(int npc_id);
	void send_move_pakcet(int npc_id);
	void send_remove_packet(int npc_id);
	void send_attack_packet(int npc_id);
	void send_rotate_packet(int npc_id);
	void send_change_animate_packet(int npc_id);
};
extern array<Monster, MAX_NPC> Monsters;
