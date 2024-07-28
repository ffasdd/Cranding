#include "pch.h"
#include "Monster.h"
#include"Session.h"
#include<limits>
array<Monster, MAX_NPC> Monsters; //전체 NPC 

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
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	_prevpos = _pos;
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange && ingamePlayer[id]->isDead == false)
	{
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
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

		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
		// 플레이어와의 최대거리가 가시거리 밖이라면 우주선을 향해 간다. 
		if (_attackState != false)
		{
			_attackState = false;
			for (auto& pl : ingamePlayer)
			{
				if (pl->_stage != ingamePlayer[id]->_stage)continue;
				pl->send_monster_attack(_id, _m_type, false);
			}
		}
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
	_pos = { 0.f,-200.f,0.f };
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

		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);
		_right = rightFloat3;

		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
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
		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{

		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
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
		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
		{
			_speed = 0;

			if (_attackState != true)
			{
				_attackState = true;
				for (auto& pl : ingamePlayer)
				{
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
					if (pl->_stage != ingamePlayer[id]->_stage)continue;
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
	//this_thread::yield();
}

bool Monster::CollideCheckToPlayer(Session* _player)
{
	if (_player->_stage != _stagenum)return false;
	if (_player->isDead == true)return false;
	if (m_SPBB.Intersects(_player->m_SPBB) == true)return true;

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
		if (_stagenum != pl->_stage ) { idx++; continue; }

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

		//cout << " 우주선 충돌 " << endl; 
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

	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{
		_fight = true;
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
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
		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
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

	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{
		_fight = true;
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
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
		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
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

	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage && ingamePlayer[id]->distance <= _viewRange)
	{
		_fight = true;
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR playerVec = XMLoadFloat3(&ingamePlayer[id]->_pos);
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
		if (CollideCheckToPlayer(ingamePlayer[id]) == true)
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
