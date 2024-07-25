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
	// Look 방향을우주선 방향으로 설정 
	// 움직이다가 플레이어가 사정거리안에 들어왔다? 
	// 플레이어 방향으로 이동 
	// 사정거리를 벗어나면 다시 우주선 방향으로 이동 
	//float checkPlayerPos = Vector3::Distance(pos);
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	_prevpos = _pos; //이동하기 이전 좌표를 저장 
	// ID가 아닌 거리와 좌표를 저장? 
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
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;
	}
	else
	{
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

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

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
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

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
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

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

		// 랜덤 attack_type 생성
		if (_m_type == MonsterType::Ice_Boss)
		{
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine generator(seed);
			std::uniform_int_distribution<int> distribution(4, 5); // 0부터 10까지의 정수 중 하나

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

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

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

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

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

		// right 벡터 계산 (look과 up 벡터의 외적)
		XMVECTOR upVec = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVec, directionToPlayer);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		// right 벡터를 업데이트
		_right = rightFloat3;

		//else 
		if (CollideCheckToPlayer(ingamePlayer[id])) _speed = 0;

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
