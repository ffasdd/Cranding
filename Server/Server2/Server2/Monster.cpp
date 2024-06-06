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

		if (_viewRange >= checkPlayerDistance) // �Ÿ��ȿ� ���Դ�. 
		{
			//cout << cl->_id << " �� Ŭ���̾�Ʈ ���� " << endl;
			XMVECTOR posVec = XMLoadFloat3(&_pos);
			XMVECTOR playerVec = XMLoadFloat3(&cl->_pos);
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
