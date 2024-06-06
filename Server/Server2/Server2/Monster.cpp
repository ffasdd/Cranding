#include "pch.h"
#include "Monster.h"
#include"Session.h"
array<Monster, MAX_NPC> Monsters; //전체 NPC 

void Monster::Move()
{
	// Look 방향을우주선 방향으로 설정 
	// 움직이다가 플레이어가 사정거리안에 들어왔다? 
	// 플레이어 방향으로 이동 
	// 사정거리를 벗어나면 다시 우주선 방향으로 이동 
	//float checkPlayerPos = Vector3::Distance(pos);
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);

	for (auto& cl : ingamePlayer)
	{

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

		float checkPlayerDistance = Vector3::Distance(_pos, cl->_pos);

		if (_viewRange >= checkPlayerDistance) // 거리안에 들어왔다. 
		{
			//cout << cl->_id << " 번 클라이언트 접근 " << endl;
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&cl->_pos);
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

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed);
			
		}
		else
		{
			_pos = Vector3::Add(_pos, directionToSpaceshipFloat3, _speed);
			
		}
	}
}

void Monster::Remove()
{
	_pos = { 0.f,-30.f,0.f };
}
