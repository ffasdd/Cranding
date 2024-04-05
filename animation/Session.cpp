#include"stdafx.h"
#include "Session.h"

Session::Session()
{
	m_name[0] = 0;
	m_hp = 0;
	m_pos = { 0.0f,0.0f,0.0f };
	m_right = { 1.0f,0.0f,0.0f };
	m_up = { 0.0f,1.0f,0.0f };
	m_look = { 0.0f,0.0f,1.0f };
}

Session::~Session()
{
}

int Session::getId()
{
	return m_id;
}

char* Session::getName()
{
	return m_name;
}

int Session::getHp()
{
	return m_hp;
}

XMFLOAT3 Session::getPos()
{
	return m_pos;
}

XMFLOAT3 Session::getLook()
{
	return m_look;
}

XMFLOAT3 Session::getRight()
{
	return m_right;
}

XMFLOAT3 Session::getUp()
{
	return m_up;
}

int Session::getAnimation()
{
	return m_animationstate;
}

int Session::getprevAnimation()
{
	return m_prevanimationstate;
}

void Session::setId(int id)
{
	m_id = id;
}

void Session::setName(char* name)
{
	strcpy_s(m_name, name);
}

void Session::setHp(int hp)
{
	m_hp = hp;
}

void Session::setPos(XMFLOAT3 pos)
{
	m_pos = pos;
}

void Session::setLook(XMFLOAT3 look)
{
	m_look = look;
}

void Session::setRight(XMFLOAT3 right)
{
	m_right = right;
}

void Session::setUp(XMFLOAT3 up)
{
	m_up = up;
}

void Session::setAnimation(int ani)
{
	m_animationstate = ani;
}

void Session::setprevAnimation(int ani)
{
	m_prevanimationstate = ani;

}
