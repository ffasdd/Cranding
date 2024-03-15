#pragma once
// Ŭ���̾�Ʈ ���� ���� , ���⼭ dosend do recv? �ʿ��Ѱ�? '

class Session
{
public:
	Session();
	~Session();
public:
	int getId();
	char* getName();
	int getHp();
	XMFLOAT3 getPos();
	XMFLOAT3 getLook();
	XMFLOAT3 getRight();
	XMFLOAT3 getUp();
	void setId(int id);
	void setName(char* name);
	void setHp(int hp);
	void setPos(XMFLOAT3 pos);
	void setLook(XMFLOAT3 look);
	void setRight(XMFLOAT3 right);
	void setUp(XMFLOAT3 up);

	
private:
	int m_id  = -1;
	char m_name[20];
	int m_hp;
	XMFLOAT3 m_pos;
	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;
	
	// state 
	// animation state;

};

