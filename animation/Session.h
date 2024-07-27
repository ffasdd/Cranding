#pragma once
#include"../Server/Server2/Server2/protocol.h"
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
	animateState getAnimation();
	animateState getprevAnimation();
	int getCharacterType();
	STATE getState();
	bool getAttack();
	bool getNpcAttack();
	bool getNpcAttacked();
	int	 getBossAttackType();
	int  getAttackPower();


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
	void setAnimation(animateState ani);
	void setprevAnimation(animateState ani);
	void setAttack(bool att);
	void setState(STATE state);
	void setPrevPos(XMFLOAT3 prevpos);
	void setNpcAttack(bool _isattack);
	void setNpcAttacked(bool _isattacked);
	void setBossAttackType(int _typenum);
	void setAttackPower(int _att);
	void Rotate(float yaw);

	mutex _sessionlock;
	mutex _uilock;
	bool m_blind = false;
	bool is_damage = false;
	int scene_num = -1;
	float m_yaw = 0.0f;


	int m_firecnt = 0;
	int m_icencnt = 0;
	int m_naturecnt = 0;

	bool is_ready = false;
private:

	int m_id  = -1;
	int r_id = -1;
	int characterType = -1;
	char m_name[20];
	int m_hp;
	int m_attpower;
	int attack_type_num = -1;


	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;
	XMFLOAT3 m_pos;
	XMFLOAT3 m_prevPos;
	bool m_isattack = false;
	bool monster_isattack = false;
	bool monster_isattacked = false;
	// state 
	STATE m_state = STATE::Free;

	animateState m_animationstate = animateState::FREE;
	animateState m_prevanimationstate = animateState::FREE;
	
	//int m_animationstate = 0;
	//int m_prevanimationstate = 0;


	

};

