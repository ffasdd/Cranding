#include "pch.h"
#include "Spaceship.h"

Spaceship::Spaceship()
{
    _pos = { 253.0f, 10.0f, 752.0f };
    _hp = 2000;
    _BoundingBox = BoundingSphere(_pos, _BoundingBoxSize);
}

Spaceship::~Spaceship()
{
}

XMFLOAT3 Spaceship::getPos()
{
    return _pos;
}

int Spaceship::getHp()
{
    return _hp;
}

BoundingSphere Spaceship::getBoundingBox()
{
    return _BoundingBox;
}

void Spaceship::setPos(XMFLOAT3 pos)
{
    _pos = pos;
}

void Spaceship::setHp(int hp)
{
    _hp = hp;
}


