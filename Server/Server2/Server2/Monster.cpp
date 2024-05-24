#include "pch.h"
#include "Monster.h"
#include"Session.h"
array<Monster, MAX_NPC> Monsters; //��ü NPC 

void Monster::Move()
{
	// Look ���������ּ� �������� ���� 
	// �����̴ٰ� �÷��̾ �����Ÿ��ȿ� ���Դ�? 
	// �÷��̾� �������� �̵� 
	// �����Ÿ��� ����� �ٽ� ���ּ� �������� �̵� 
	//float checkPlayerPos = Vector3::Distance(pos);
	for (auto& cl : ingamePlayer)
	{
		float checkPlayerDistance = Vector3::Distance(_pos, cl->_pos);
		if (_viewRange >= checkPlayerDistance) // �Ÿ��ȿ� ���Դ�. 
		{
			cout << cl->_id << " �� Ŭ���̾�Ʈ ���� " << endl;
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
