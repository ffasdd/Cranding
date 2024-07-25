#include "pch.h"
#include "Monster.h"
#include"Session.h"
#include<limits>
array<Monster, MAX_NPC> Monsters; //��ü NPC 

FireBossMonster _FireBoss;
IceBossMonster _IceBoss;
NatureBossMonster _NatureBoss;

Monster::Monster() : ingamePlayer{ nullptr,nullptr }
{
	_pos = { 0.f,-150.0f,0.f };
	_initPos = { 0.f,-150.0f,0.f };
	_prevpos = { 0.f,-150.0f,0.f };
	_right = { 1.f,0.0f,0.f };
	_up = { 0.f,1.0f,0.f };
	_look = { 0.f,0.0f,1.f };

	_hp = -1;
	_att = -1;
	_stagenum = -1;
	_id = -1;

}

void Monster::Move()
{
	// Look ���������ּ� �������� ���� 
	// �����̴ٰ� �÷��̾ �����Ÿ��ȿ� ���Դ�? 
	// �÷��̾� �������� �̵� 
	// �����Ÿ��� ����� �ٽ� ���ּ� �������� �̵� 
	//float checkPlayerPos = Vector3::Distance(pos);
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	_prevpos = _pos; //�̵��ϱ� ���� ��ǥ�� ���� 
	// ID�� �ƴ� �Ÿ��� ��ǥ�� ����? 
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;
	}
	else
	{
		// �÷��̾���� �ִ�Ÿ��� ���ðŸ� ���̶�� ���ּ��� ���� ����. 
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR spaceshipVec = XMLoadFloat3(&_spaceship->getPos());
		XMVECTOR dirToSpaceship = XMVector3Normalize(spaceshipVec - posVec);

		XMFLOAT3 directionToSpaceshipFloat3;
		XMStoreFloat3(&directionToSpaceshipFloat3, dirToSpaceship);

		_look = directionToSpaceshipFloat3;

		XMVECTOR upVect = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVect, dirToSpaceship);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		_right = rightFloat3;
		if (CollideCheckToSpaceship()) _speed = 0;
		_pos = Vector3::Add(_pos, directionToSpaceshipFloat3, _speed);
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;

	}
}


void Monster::Remove()
{
	_pos = { 0.f,-100.f,0.f };
}





void Monster::IceMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

		_speed = 0.7f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;

	}
	else
	{
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right ���� ��� (look�� up ������ ����)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right ���͸� ������Ʈ
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

void Monster::FireMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

		_speed = 1.2f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right ���� ��� (look�� up ������ ����)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right ���͸� ������Ʈ
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

void Monster::NatureMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right ���� ��� (look�� up ������ ����)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right ���͸� ������Ʈ
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

void Monster::NightAttack(int cl_id)
{
	cout << "Attack " << endl;
	_attackState = true;
	//this_thread::yield();
}

bool Monster::CollideCheckToPlayer(Session* _player)
{
	if (_player->_stage != _stagenum)return false;

	if (m_SPBB.Intersects(_player->m_SPBB) == true)
	{
		if (_attackState == true) {
			return true;
		}
		_attackState = true;

		SC_MONSTER_ATTACK_PACKET p;
		p.size = sizeof(SC_MONSTER_ATTACK_PACKET);
		p.type = SC_MONSTER_ATTACK;
		p.monstertype = _m_type;
		p.id = _id;
		p.is_attack = _attackState;

		// ���� attack_type ����
		if (_m_type == MonsterType::Ice_Boss)
		{
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine generator(seed);
			std::uniform_int_distribution<int> distribution(4, 5); // 0���� 10������ ���� �� �ϳ�

			p.attack_type = distribution(generator);
		}
		for (auto& pl : ingamePlayer)
		{
			if (pl->_stage != _player->_stage)continue;
			pl->do_send(&p);
		}
		//_player->do_send(&p);

		return true;
	}
	else
	{
		if (_attackState != false)
		{
			if (_attackState == false)
			{
				return false;
			}
			_attackState = false;

			SC_MONSTER_ATTACK_PACKET p;
			p.size = sizeof(SC_MONSTER_ATTACK_PACKET);
			p.type = SC_MONSTER_ATTACK;
			p.monstertype = _m_type;
			p.id = _id;
			p.is_attack = _attackState;

			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != _player->_stage)continue;
				pl->do_send(&p);
			}
			//_player->do_send(&p);
		}
	}
	return false;
}

int Monster::FindClosePlayer()
{
	int closestPlayerId = -1;
	int farthestPlayerId = -1;

	float minDistance = 50000.0f;
	float maxDistance = 0.0f;

	int  idx = 0;
	for (auto& pl : ingamePlayer)
	{
		if (_stagenum != pl->_stage) { idx++; continue; }
		pl->distance = Vector3::Distance(pl->_pos, _pos);

		if (pl->distance < minDistance)
		{
			minDistance = pl->distance;
			closestPlayerId = idx;
		}

		idx++;
	}

	return closestPlayerId;
}

bool Monster::CollideCheckToSpaceship()
{
	if (m_SPBB.Intersects(_spaceship->getBoundingBox()) == true)
	{
		if (_spaceshipattackState == true) {
			return true;
		}
		_spaceshipattackState = true;

		SC_MONSTER_ATTACK_PACKET p;
		p.size = sizeof(SC_MONSTER_ATTACK_PACKET);
		p.type = SC_MONSTER_ATTACK;
		p.monstertype = _m_type;
		p.id = _id;
		p.is_attack = _spaceshipattackState;
		for (auto& pl : ingamePlayer)
		{
			if (pl->_stage != 2)continue;
			pl->do_send(&p);
		}
		//_player->do_send(&p);

		//cout << " ���ּ� �浹 " << endl; 
		return true;

	}
	else
	{
		if (_spaceshipattackState != false)
		{
			_spaceshipattackState = false;

			SC_MONSTER_ATTACK_PACKET p;
			p.size = sizeof(SC_MONSTER_ATTACK_PACKET);
			p.type = SC_MONSTER_ATTACK;
			p.monstertype = _m_type;
			p.id = _id;
			p.is_attack = _spaceshipattackState;

			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != 2)continue;
				pl->do_send(&p);
			}
		}
	}
	return false;

}

IceBossMonster::IceBossMonster()
{
	_look = { 0.f,0.f,1.f };
	_right = { 1.f,0.f,0.f };
	_up = { 0.f,1.f,0.f };
}

void IceBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;

	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 
		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right ���� ��� (look�� up ������ ����)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right ���͸� ������Ʈ
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

FireBossMonster::FireBossMonster()
{
	_look = { 0.f,0.f,1.f };
	_right = { 1.f,0.f,0.f };
	_up = { 0.f,1.f,0.f };
}

void FireBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right ���� ��� (look�� up ������ ����)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right ���͸� ������Ʈ
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

NatureBossMonster::NatureBossMonster()
{
	_look = { 0.f,0.f,1.f };
	_right = { 1.f,0.f,0.f };
	_up = { 0.f,1.f,0.f };
}

void NatureBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right ���� ��� (look�� up ������ ����)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right ���͸� ������Ʈ
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right ���� ��� (look�� up ������ ����)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right ���͸� ������Ʈ
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}
