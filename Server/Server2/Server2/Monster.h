#pragma once
class Session;
enum class MonsterType : char;
class Monster
{
	//Monster() = delete;

public:

	XMFLOAT3 _pos;
	XMFLOAT3 _prevpos;
	XMFLOAT3 _right;
	XMFLOAT3 _up;
	XMFLOAT3 _look;
	XMFLOAT3 _velocity;

	XMFLOAT3 spaceshippos = { 250.0f,10.0f,750.0f };

	float _speed = 1.0f;
	float _viewRange = 150.0f;
	float _attackRange = 50.0f;

	int _hp;
	int _att;

	int _stagenum;
	int _id;
	bool _is_active = false;
	bool _is_alive = false;

	bool _attackState = false;
	MonsterType _m_type;

	float                           m_fBoundingSize{ 3.0f };
	BoundingSphere					m_SPBB = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), m_fBoundingSize);
	
	mutex ingamePlayerlock;

	mutex MonsterLock;


public:

	//vector<Session*> ingamePlayer;
	array<Session*, 2> ingamePlayer;
	vector<pair<float, XMFLOAT3>> playerdisPos;
public:


	virtual void Move();
	void Remove();

	void IceMove();
	void FireMove();
	void NatureMove();

	void NightAttack(int cl_id);

	bool CollideCheckToPlayer();

	// trace
	int FindClosePlayer();

	


};

class FireBossMonster : public Monster
{
public:
	FireBossMonster();
	void Move() override;

};

class IceBossMonster : public Monster
{
public:
	IceBossMonster();
	void Move() override;
};

class NatureBossMonster : public Monster
{
public:
	NatureBossMonster();
	void Move() override;
};
extern array<Monster, MAX_NPC> Monsters;
