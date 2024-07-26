#pragma once
class Spaceship
{
public:
	Spaceship();
	~Spaceship();

public:
	XMFLOAT3 getPos();
	int getHp();
	BoundingSphere getBoundingBox();

	void setPos(XMFLOAT3 pos);
	void setHp(int hp);

private:

	XMFLOAT3 _pos;
	int _hp;

	float _BoundingBoxSize = 70.0f;
	BoundingSphere _BoundingBox;

};

