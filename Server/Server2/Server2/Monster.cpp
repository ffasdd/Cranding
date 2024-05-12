#include "pch.h"
#include "Monster.h"
#include"protocol.h"

array<Monster, MAX_NPC> Monsters; //ÀüÃ¼ NPC 

void Monster::Move()
{
	_pos = Vector3::Add(_pos, _look, _speed);

}

void Monster::Remove()
{
	_pos = { 0.f,0.f,0.f };
}
