#pragma once
class Session;

class Monster
{
	//Monster() = delete;

public:

	XMFLOAT3 _pos;
	XMFLOAT3 _right;
	XMFLOAT3 _up;
	XMFLOAT3 _look;
	XMFLOAT3 _velocity;

	XMFLOAT3 spaceshippos = { 250.0f,10.0f,750.0f };

	float _speed = 1.0f;
	float _viewRange = 150.0f;
	int _hp;
	int _att;

	int _id;
	bool _is_active = false;
	bool _is_alive = false;

	BoundingOrientedBox _ob;
	mutex ingamePlayerlock;

public:
	//array<Seesion*, 3> ing 
	vector<Session*> ingamePlayer;

public:

	void Move();
	void Remove();
	void RemovePlayer(int client_id);
};
extern array<Monster, MAX_NPC> Monsters;
