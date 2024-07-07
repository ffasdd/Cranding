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
	_prevpos = _pos; //�̵��ϱ� ���� ��ǥ�� ���� 
	for (auto& cl : ingamePlayer)
	{
		if (cl->_stage != _stagenum)continue;
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

		float checkPlayerDistance = Vector3::Distance(_pos, cl->_pos); // ����ڿ��� ���� �Ÿ� 

		if (_viewRange >= checkPlayerDistance) // �Ÿ��ȿ� ���Դ�. 
		{
			
			//if (cl->_stage != 2)
			//{
			//	// ���� ���������� �ƴҶ� ������ ���ϰ� ���Ƴ����� 
			//	_pos = Vector3::Add(_pos, directionToSpaceshipFloat3, _speed);

			//}//cout << cl->_id << " �� Ŭ���̾�Ʈ ���� " << endl;
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
			//if (CollideCheckToPlayer()) _pos = _prevpos;
			//else 
			
			_pos = Vector3::Add(_pos, directionToPlayerFloat3, _speed); // �̵� , 

			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
			
		}
		else
		{
			_pos = Vector3::Add(_pos, directionToSpaceshipFloat3, _speed);
			m_SPBB.Center = _pos;
			m_SPBB.Center.y = _pos.y;
	;
		}
	}
}

void Monster::Remove()
{
	_pos = { 0.f,-100.f,0.f };
}

void Monster::RemovePlayer(int client_id)
{
	Session* removeplayer;
	auto it = find_if(ingamePlayer.begin(), ingamePlayer.end(), [client_id](Session* s) {return s->_id == client_id; });
	//���信�� Ŭ���̾�Ʈ ���̵� ������
	if (ingamePlayer.end() != it)
	{
		ingamePlayer.erase(it);
	}
}



void Monster::IceMove()
{
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
			// �ٽ� �ʱ�ȭ ��ġ��? �ƴϸ� ����? 

		}
	}
}

bool Monster::CollideCheckToPlayer()
{
	for (auto& pl : ingamePlayer)
	{
		if (m_SPBB.Intersects(pl->m_SPBB) == true)
			return true;
	}
	return false;
}
