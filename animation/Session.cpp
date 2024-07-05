#include"stdafx.h"
#include "Session.h"

Session::Session()
{
	m_name[0] = 0;
	m_hp = 0;
	m_pos = { 0.0f,-100.0f,0.0f };
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

int Session::getrId()
{
	return r_id;
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

XMFLOAT3 Session::getPrevPos()
{
	return m_prevPos;
}

int Session::getAnimation()
{
	return m_animationstate;
}

int Session::getprevAnimation()
{
	return m_prevanimationstate;
}

int Session::getCharacterType()
{
	return characterType;
}

STATE Session::getState()
{
	return m_state;
}

bool Session::getAttack()
{
	return m_isattack;
}

void Session::setCharacterType(int type)
{
	characterType = type;
}

void Session::setId(int id)
{
	m_id = id;
}

void Session::setrId(int roomid)
{
	r_id = roomid;
}

void Session::setName(char* name)
{
	strcpy_s(m_name, name);
}

void Session::setHp(int hp)
{
	m_hp = hp;
}

void Session::setPos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;

}

void Session::setPos(XMFLOAT3 pos)
{
	m_pos = pos;
}

void Session::setLook(float lx, float ly, float lz)
{
	m_look.x = lx;
	m_look.y = ly;
	m_look.z = lz;
}

void Session::setLook(XMFLOAT3 look)
{
	m_look = look;
}

void Session::setRight(float rx,float ry,float rz)
{
	m_right.x = rx;
	m_right.y = ry;
	m_right.z = rz;
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

void Session::setAttack(bool att)
{
	m_isattack = att;
}

void Session::setState(STATE state)
{
	m_state = state;
}

void Session::setPrevPos(XMFLOAT3 prevpos)
{
	m_prevPos = prevpos;
}

void Session::Rotate(float yaw)
{
	if (yaw != 0.0f)
	{
		m_yaw += yaw;
		if (m_yaw > 360.0f) m_yaw -= 360.0f;
		if (m_yaw < 0.0f)m_yaw += 360.0f;

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_up), XMConvertToRadians(yaw));
		m_look = Vector3::TransformNormal(m_look, xmmtxRotate);
		m_right = Vector3::TransformNormal(m_right, xmmtxRotate);

		m_look = Vector3::Normalize(m_look);
		m_right = Vector3::CrossProduct(m_up, m_look, true);
		m_up = { 0.0f,1.0f,0.0f };
		//m_xmf3Up = Vector3::CrossProduct(m_look, m_right, true);
	}
}
