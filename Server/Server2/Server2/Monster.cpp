#include "pch.h"
#include "Monster.h"
#include"protocol.h"

array<Monster, MAX_NPC> Monsters; //��ü NPC 

void Monster::Move()
{
	_pos = Vector3::Add(_pos, _look, _speed);

}
