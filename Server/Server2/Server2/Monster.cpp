#include "pch.h"
#include "Monster.h"
#include"Session.h"
#include<limits>
array<Monster, MAX_NPC> Monsters; //전체 NPC 

FireBossMonster _FireBoss;
IceBossMonster _IceBoss;
NatureBossMonster _NatureBoss;

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
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage)
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
		if (CollideCheckToPlayer()) _speed = 0;

		_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

		_speed = 1.0f;
		m_SPBB.Center = _pos;
		m_SPBB.Center.y = _pos.y;
	}
	else
	{
		// 플레이어와의 최대거리가 가시거리 밖이라면 우주선을 향해 간다. 
		XMVECTOR posVec = XMLoadFloat3(&_pos);
		XMVECTOR spaceshipVec = XMLoadFloat3(&spaceshippos);
		XMVECTOR dirToSpaceship = XMVector3Normalize(spaceshipVec - posVec);

		XMFLOAT3 directionToSpaceshipFloat3;
		XMStoreFloat3(&directionToSpaceshipFloat3, dirToSpaceship);

		_look = directionToSpaceshipFloat3;

		XMVECTOR upVect = XMLoadFloat3(&up);
		XMVECTOR rightVec = XMVector3Cross(upVect, dirToSpaceship);
		XMFLOAT3 rightFloat3;
		XMStoreFloat3(&rightFloat3, rightVec);

		_right = rightFloat3;

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
	if (id != -1 && _stagenum== ingamePlayer[id]->_stage)
	{
		for (auto& cl : ingamePlayer)
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
			if (CollideCheckToPlayer()) _speed = 0;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

			_speed = 1.0f;
			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;

		}
	}
	else
	{

	}
}

void Monster::FireMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();
	if (id != -1 && _stagenum == ingamePlayer[id]->_stage)
	{
		for (auto& cl : ingamePlayer)
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
			if (CollideCheckToPlayer()) _speed = 0;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

			_speed = 1.0f;
			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;

		}
	}
	else
	{

	}
}

void Monster::NatureMove()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
	int id = FindClosePlayer();

	if (id != -1 && _stagenum == ingamePlayer[id]->_stage)
	{
		for (auto& cl : ingamePlayer)
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
			if (CollideCheckToPlayer()) _speed = 0;

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // 이동 , 

			_speed = 1.0f;
			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;

		}
	}
	else
	{

	}
}

bool Monster::CollideCheckToPlayer()
{
	for (auto& pl : ingamePlayer)
	{
		if (pl->_stage != _stagenum)continue;
		if (m_SPBB.Intersects(pl->m_SPBB) == true)
			return true;
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
		if (_stagenum != pl->_stage)continue;
		pl->distance = Vector3::Distance(pl->_pos, _pos);
		if (pl->distance <= _viewRange)
		{
			if (pl->distance < minDistance)
			{
				minDistance = pl->distance;
				closestPlayerId = idx;
			}
		}
		idx++;
	}

	return closestPlayerId;
}

void FireBossMonster::Move()
{
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	_prevpos = _pos;
}

void IceBossMonster::Move()
{
}

void NatureBossMonster::Move()
{
}
