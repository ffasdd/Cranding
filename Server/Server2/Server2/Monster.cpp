#include "pch.h"
#include "Monster.h"
#include"protocol.h"

array<Monster, MAX_NPC> Monsters; //ÀüÃ¼ NPC 

void Monster::Move()
{
	//if (_pos.z > 720.0f) _pos = Vector3::Add(_pos, XMFLOAT3{ 0.f,0.f,-1.f }/*_look*/, _speed);
	//else if( _pos.z < -240 ) _pos = Vector3::Add(_pos, _look, _speed);
	_pos = Vector3::Add(_pos, _look, _speed);
}

void Monster::Remove()
{
	_pos = { 0.f,-30.f,0.f };
}
