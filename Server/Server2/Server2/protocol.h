constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 1000;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;

constexpr int MAX_USER = 15;
constexpr int MAX_ROOM = 5;

constexpr int MAX_ROOM_USER = 2;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;
constexpr char CS_TELEPORT = 4;
constexpr char CS_LOGOUT = 5;
constexpr char CS_ROTATE = 6;
constexpr char CS_CHANGE_ANIMATION = 7;
constexpr char CS_READY_GAME = 8;
constexpr char CS_START_GAME = 9;
constexpr char CS_CHANGE_SCENE = 10;
constexpr char CS_INGAME_START = 11;
constexpr char CS_TIME_CHECK = 12;
constexpr char CS_MOVE_MONSTER = 13;
constexpr char CS_ATTACK_COLLISION = 14;
constexpr char CS_ICE_MONSTER_UPDATE = 15;
constexpr char CS_MONSTER_DIE = 16;


constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7;
constexpr char SC_LOGIN_FAIL = 8;
constexpr char SC_STAT_CHANGE = 9;
constexpr char SC_ROTATE_OBJECT = 10;
constexpr char SC_CHANGE_ANIMATION = 13;
constexpr char SC_START_GAME = 14;
constexpr char SC_CHANGE_SCENE = 15;
constexpr char SC_ATTACK = 16;
constexpr char SC_INGAME_STRAT = 17;
constexpr char SC_TIME_CHECK = 18;
constexpr char SC_ADD_MONSTER = 19;
constexpr char SC_MOVE_MONSTER = 20;
constexpr char SC_ATTACK_COLLISION = 22;
constexpr char SC_DAYTIME = 23;
constexpr char SC_NIGHT = 24;
constexpr char SC_MONSTER_UPDATE_POS = 25;
constexpr char SC_MONSTER_UPDATE_LOOK = 26;
constexpr char SC_MONSTER_UPDATE_RIGHT = 27;
constexpr char SC_ICE_MONSTER_UPDATE = 28;
constexpr char SC_FIRE_MONSTER_UPDATE = 29;
constexpr char SC_NATURE_MONSTER_UPDATE = 30;
constexpr char SC_ICE_BOSS_UPDATE = 31;
constexpr char SC_FIRE_BOSS_UPDATE = 32;
constexpr char SC_NATURE_BOSS_UPDATE = 33;
constexpr char SC_MONSTER_ATTACK = 34;
constexpr char SC_MONSTER_DIE = 35;




constexpr char CS_TEST = 11;
constexpr char SC_TEST = 12; 




constexpr float VIEW_RANGE = 1000.0f;

enum class MonsterType : char {
	Fire,
	Ice,
	Nature,
	Night,
	Fire_Boss,
	Ice_Boss,
	Nature_Boss
};

enum class sceneState :char {
	LOBBY,
	STARTSHIP,
	ICE,
	FIRE,
	NATURE
};
enum class animateState : int {
	SWORD_IDLE,
	SWORD_MOVE,
	TAKE_DAMAGE,
	BLACKOUT,
	STAND_UP,
	HEAL,
	SWORD_ATTACK_1,
	SWORD_ATTACK_2,
	FREE
};
enum class animateState_NPC : char {
	FIRE_ATTACK,
	ICE_ATTACK,
	NATURE_ATTACK,
	FIRE_BOSS_ATTACK,
	ICE_BOSS_ATTACK,
	NATURE_BOSS_ATTACK
};

#pragma pack (push, 1)
struct NightMonster
{
	float _x;
	float _y;
	float _z;

	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	//XMFLOAT3 _pos; // 12 
	//XMFLOAT3 _look; // 12 
	//XMFLOAT3 _right; // 12 
	int _id; // 4
	// 40 
};
struct NightMonstersUpdate
{
	
	unsigned char size; // 1
	char type; // 1 
	NightMonster _monster; //  400 
	// float 
	// 메모리잡는방식. 
	// 
	// 402  보내는건 142  
};
struct IceMonster
{
	float _x;
	float _y;
	float _z;

	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	int  _id;
};

struct IceMonstersUpdate
{
	unsigned char size;
	char type;
	IceMonster _monster;
};

struct IceBossMonsterStruct
{
	float _x;
	float _y;
	float _z;

	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	int  _id;
};
struct BossUpdate_Ice
{
	unsigned char size;
	char type;
	IceBossMonsterStruct _boss;
};

struct Firemonster
{
	float _x;
	float _y;
	float _z;

	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	int  _id;
};


struct FireMonsterUpdate
{
	unsigned char size;
	char type;
	Firemonster _monster;
};

struct FireBossMonsterStruct
{
	float _x;
	float _y;
	float _z;


	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	int  _id;
};
struct BossUpdate_Fire
{
	unsigned char size;
	char type;
	FireBossMonsterStruct _boss;
};

struct NatureMonster
{
	float _x;
	float _y;
	float _z;

	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	int  _id;
};

struct NatureMonsterUpdate
{
	unsigned char size;
	char type;
	NatureMonster _monster;
};

struct NatureBossMonsterStruct
{
	float _x;
	float _y;
	float _z;


	float _lx;
	float _ly;
	float _lz;

	float _rx;
	float _ry;
	float _rz;

	int  _id;
};
struct BossUpdate_Nature
{
	unsigned char size;
	char type;
	NatureBossMonsterStruct _boss;
}; 
struct NightMonsterLook
{
	int _id; 
	XMFLOAT3 _look;
};
struct NightMonsterRight
{
	int _id;
	XMFLOAT3 _right;
};


struct NightMonstersLook
{
	unsigned char size;
	char type;
	NightMonsterLook _monster[10];
};
struct NightMonstersRight
{
	unsigned char size;
	char type;
	NightMonsterRight _monster[10];
};

struct NightMonstersDead
{
	unsigned char size;
	char type;
	NightMonster _monster[10];
};
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	int		id;
	int		charactertype;
};
struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	DWORD	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	XMFLOAT3 pos;
	animateState a_state;
	unsigned	move_time;
	int		roomid;
};
struct CS_CHAT_PACKET {
	unsigned char size;
	char	type;
	char	mess[CHAT_SIZE];
};
struct CS_LOGOUT_PACKET {
	unsigned char size;
	char	type;
};


struct CS_ROTATE_PACKET {
	unsigned char size;
	char type;
	float yaw;
	int		roomid;
};
struct CS_CHANGE_ANIMATION_PACKET {
	unsigned char size;
	char type;
	animateState a_state;
	animateState prev_a_state;
	int		roomid;
};
struct CS_READY_PACKET {
	unsigned char size;
	char type;

};
struct CS_START_PACKET {
	unsigned char size;
	char type;
};
struct CS_CHANGE_SCENE_PACKET {
	unsigned char size;
	char type;
	int scenenum;
	int roomid;

};
struct CS_INGAME_START_PACKET {
	unsigned char size;
	char type;
	int roomid;
	
};
struct CS_ATTACK_PACKET {
	unsigned char size;
	char type;
	int roomid;
	bool isAttack;
};
struct CS_ATTACK_COLLISION_PACKET {
	unsigned char size;
	char type;
	MonsterType _montype;
	int npc_id;
	int room_id;

};
struct CS_MONSTER_DIE_PACKET {
	unsigned char size;
	char type;
	int npc_id;
	MonsterType _montype;
	int room_id;
};
struct CS_TIME_CHECK_PACKET {
	unsigned char size;
	char type;
	int roomid;
	int time;

};

struct SC_GAMESTART_PACKET {
	unsigned char size;
	char type;
	int		id;
	int		roomid;
	
};
struct SC_LOGIN_INFO_PACKET {

	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
	int		charactertype;
	int		room_id;
	int		stage_num;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 right;
	XMFLOAT3 up;

	animateState a_state;
	animateState prev_state;
};
struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		Max_hp;
	int		charactertype;
	int		stage_num;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 right;
	XMFLOAT3 up;
	char	name[NAME_SIZE];
	int		roomid;
	animateState a_state;
	animateState prev_state;
};
struct SC_ADD_MONSTER_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		monstertype;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 right;
	XMFLOAT3 up;
	int		roomid;
	animateState a_state;
	animateState prev_state;
};
struct SC_MOVE_MONSTER_PACKET {
	unsigned char size;
	char type;
	int id;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 right;


};
struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
};
struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	XMFLOAT3 pos;
};
//struct SC_ROTATE_OBJECT_PACKET {
//	unsigned char size;
//	char type;
//	XMFLOAT3 look;
//	XMFLOAT3 right;
//	XMFLOAT3 up;
//	int id;
//};
struct SC_ROTATE_OBJECT_PACKET {
	unsigned char size;
	char type;
	float yaw;
	int id;
};
struct SC_CHAT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	char	mess[CHAT_SIZE];
};
struct SC_MONSTER_ATTACK_PACKET {
	unsigned char size;
	char type;
	int id;
	bool is_attack;
	MonsterType monstertype;
	animateState_NPC a_state;
};
struct SC_CHANGE_ANIMATION_PACKET {
	unsigned char size;
	char type;
	int id;

	animateState a_state;
	animateState prev_a_state;
};
struct SC_LOGIN_FAIL_PACKET {
	unsigned char size;
	char	type;

};
struct SC_CHANGE_SCENE_PACKET {
	unsigned char size;
	char type;
	int stage;
	int id;
	XMFLOAT3 pos;
};
struct SC_INGAME_START_PACKET {
	unsigned char size;
	char type;
};
struct SC_ATTACK_PACKET {
	unsigned char size;
	char type;
	int id;
	bool isAttack;
};
struct SC_TIME_CHECK_PACKET {
	unsigned char size;
	char type;
	int time;
};
struct SC_DAYTIME_PACKET {
	unsigned char size;
	char type;; 
	int firecnt;
	int icecnt;
	int naturecnt;

};
struct SC_NIGHT_PACKET {
	unsigned char size;
	char type;
};

struct SC_MONSTER_DIE_PACKET {
	unsigned char size;
	char type;
	MonsterType _montype;
	int npc_id;
	bool _isattacked;
};


struct CS_TEST_PACKET {
	unsigned char size;
	char	type;
	int id;
};
struct SC_TEST_PACKET {
	unsigned char size;
	char	type;

};	
#pragma pack (pop)