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

	BoundingOrientedBox _ob;
};
extern array<Monster, MAX_NPC> Monsters;
