#pragma once
// 클라이언트 들의 정보 , 여기서 dosend do recv? 필요한가? '
enum class animateState;
enum class STATE : unsigned int { Free, Alloc, Ingame, Start };
class Session
{
public:
	Session();
	~Session();
public:
	int getId();
	int getrId();
	char* getName();
	int getHp();
	XMFLOAT3 getPos();
	XMFLOAT3 getLook();
	XMFLOAT3 getRight();
	XMFLOAT3 getUp();
	XMFLOAT3 getPrevPos();
	int getAnimation();
	int getprevAnimation();
	int getCharacterType();
	STATE getState();
	bool getAttack();


	void setCharacterType(int type);
	void setId(int id);
	void setrId(int roomid);
	void setName(char* name);
	void setHp(int hp);
	void setPos(float x, float y, float z);
	void setPos(XMFLOAT3 pos);
	void setLook(float lx, float ly, float lz);
	void setLook(XMFLOAT3 look);
	void setRight(float rx, float ry, float rz);
	void setRight(XMFLOAT3 right);
	void setUp(XMFLOAT3 up);
	void setAnimation(int ani);
	void setprevAnimation(int ani);
	void setAttack(bool att);
	void setState(STATE state);
	void setPrevPos(XMFLOAT3 prevpos);
	void Rotate(float yaw);

	mutex _sessionlock;
	bool m_blind = false;
	int scene_num = -1;
	float m_yaw = 0.0f;

private:

	int m_id  = -1;
	int r_id = -1;
	int characterType = -1;
	char m_name[20];
	int m_hp;

	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;
	XMFLOAT3 m_pos;
	XMFLOAT3 m_prevPos;
	bool m_isattack = false;
	bool monster_isattack = false;
	// state 
	STATE m_state = STATE::Free;

	int m_animationstate = 0;
	int m_prevanimationstate = 0;


	

};

