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
	for (auto& cl : ingamePlayer)
	{
		float checkPlayerDistance = Vector3::Distance(_pos, cl->_pos);
		if (_viewRange >= checkPlayerDistance) // 거리안에 들어왔다. 
		{
			cout << cl->_id << " 번 클라이언트 접근 " << endl;
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&cl->_pos);
			XMVECTOR directionToPlayer = XMVector3Normalize(playerVec - posVec);

			XMFLOAT3 directionToPlayerFloat3;
			XMStoreFloat3(&directionToPlayerFloat3, directionToPlayer);

			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed);
			
		}
		else
		{
			_pos = Vector3::Add(_pos, _look, _speed);
		}
	}
}

void Monster::Remove()
{
	_pos = { 0.f,-30.f,0.f };
}
