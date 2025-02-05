#include "pch.h"
#include "Monster.h"
#include"Session.h"
#include<limits>
array<Monster, MAX_NPC> Monsters; //전체 NPC 

FireBossMonster _FireBoss;
IceBossMonster _IceBoss;
NatureBossMonster _NatureBoss;

Monster::Monster() 
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
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	_prevpos = _pos;
	Session* closetPlayer= FindClosePlayer();

	if (closetPlayer != nullptr && closetPlayer->isDead == false)
	{
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);
		// right 벡터를 업데이트
		_right = rightFloat3;

		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;

			if (_attackState == false)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != closetPlayer->_stage)continue;
					pl->send_monster_attack(_id, _m_type, true);
				}
			}
		}
		else
		{
			if (_attackState == true)
			{
				_attackState = false;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != closetPlayer->_stage)continue;
					pl->send_monster_attack(_id, _m_type, false);
				}
			}
		}

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

		_speed = 0.5f;

		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;
	}
	else
	{
		if (_attackState == true)
		{
			_attackState = false;
			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != _stagenum)continue;
				pl->send_monster_attack(_id, _m_type, false);
			}
		}
		// 플레이어와의 최대거리가 가시거리 밖이라면 우주선을 향해 간다. 
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
	_pos.y = -1500;
}

void Monster::MarkAsDead()
{
	bool expected = true;
	if (_is_alive.compare_exchange_strong(expected, false)) {
		Remove();
	}

}

void Monster::IceMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	Session* closetPlayer = FindClosePlayer();
	if (closetPlayer != nullptr)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);
		_right = rightFloat3;

		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != _stagenum)continue;
					pl->send_monster_attack(_id, _m_type, true);
				}
			}
		}
		else
		{
			if (_attackState != false)
			{
				_attackState = false;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != _stagenum)continue;
					pl->send_monster_attack(_id, _m_type, false);
				}
			}
		}


		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

		m_SPBB.Center = _pos;
		m_SPBB.Center.y = 20.0f;

		_speed = 0.7f;

	}
	else
	{
		if (_attackState == true)
		{
			_attackState = false;
			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != _stagenum)continue;
				pl->send_monster_attack(_id, _m_type, false);
			}
		}
		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right 벡터 계산 (look과 up 벡터의 외적)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right 벡터를 업데이트
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = 0.0f;
		}
	}
}

void Monster::FireMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	Session* closetPlayer = FindClosePlayer();
	if (closetPlayer != nullptr)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != -_stagenum)continue;
					pl->send_monster_attack(_id, _m_type, true);
				}
			}
		}
		else
		{
			if (_attackState != false)
			{
				_attackState = false;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != _stagenum)continue;
					pl->send_monster_attack(_id, _m_type, false);
				}
			}
		}
		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

		_speed = 0.6f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{

		if (_attackState == true)
		{
			_attackState = false;
			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != _stagenum)continue;
				pl->send_monster_attack(_id, _m_type, false);
			}
		}

		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right 벡터 계산 (look과 up 벡터의 외적)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right 벡터를 업데이트
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

void Monster::NatureMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	Session* closetPlayer = FindClosePlayer();

	if (closetPlayer != nullptr)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage !=_stagenum)continue;
					pl->send_monster_attack(_id, _m_type, true);
				}
			}
		}
		else
		{
			if (_attackState != false)
			{
				_attackState = false;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != _stagenum)continue;
					pl->send_monster_attack(_id, _m_type, false);
				}
			}
		}

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;


	}
	else
	{
		if (_attackState == true)
		{
			_attackState = false;
			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != _stagenum)continue;
				pl->send_monster_attack(_id, _m_type, false);
			}
		}

		if (int(_pos.x) != int(_initPos.x) && int(_pos.z) != int(_initPos.z))
		{
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&_initPos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_look = directionToPlayerFloat3;

			// right 벡터 계산 (look과 up 벡터의 외적)
			XMVECTOR upVec = XMLoadFloat3(&up);
			XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
			XMFLOAT3 rightFloat3;
			XMStoreFloat3(&rightFloat3, rightVec);

			// right 벡터를 업데이트
			_right = rightFloat3;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
		}
	}
}

void Monster::NightAttack(int cl_id)
{
	cout << "Attack " << endl;
	_attackState = true;
}

bool Monster::CollideCheckToPlayer(Session* _player)
{
	if (_player->_stage != _stagenum)return false;
	if (m_SPBB.Intersects(_player->m_SPBB) == true)return true;

	return false;
}

Session* Monster::FindClosePlayer()
{
	
	float minDistance = 50000.0f;
	Session* closetPlayer = nullptr;

	for (auto& pl : ingamePlayer)
	{
		if (_stagenum != pl->_stage) continue;
		if (_viewRange < Vector3::Distance(pl->_pos, _pos)) continue;
		if (pl->distance < minDistance)
		{
			minDistance = pl->distance;
			closetPlayer = pl;
		}
	}
	return closetPlayer;
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
	_hp = 200;
}

void IceBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;

	Session* closetPlayer = FindClosePlayer();

	if (closetPlayer != nullptr)
	{
		_fight = true;
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;
		}

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 
		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;

	}
	else
	{
		_fight = false;
	}
}

FireBossMonster::FireBossMonster()
{
	_look = { 0.f,0.f,1.f };
	_right = { 1.f,0.f,0.f };
	_up = { 0.f,1.f,0.f };
	_hp = 200;
}

void FireBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;

	Session* closetPlayer = FindClosePlayer();

	if (closetPlayer != nullptr)
	{
		_fight = true;
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;

		}
		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 
		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;
	}
	else
	{
		_fight = false;
	}
}

NatureBossMonster::NatureBossMonster()
{
	_look = { 0.f,0.f,1.f };
	_right = { 1.f,0.f,0.f };
	_up = { 0.f,1.f,0.f };
	_hp = 200;
}

void NatureBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;

	Session* closetPlayer = FindClosePlayer();

	if (closetPlayer != nullptr )
	{
		_fight = true;
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&closetPlayer->_pos);
		XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

		XMFLOAT3 directionToPlayerFloat3;
		XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

		_look = directionToPlayerFloat3;

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(closetPlayer) == true)
		{
			_speed = 0;

		}
		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 
		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;
	}
	else
	{
		_fight = false;
	}
}
