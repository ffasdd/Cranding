constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;

constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 200000;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;
constexpr char CS_TELEPORT = 4;
constexpr char CS_LOGOUT = 5;
constexpr char CS_ROTATE = 6;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7;
constexpr char SC_LOGIN_FAIL = 8;
constexpr char SC_STAT_CHANGE = 9;
constexpr char SC_ROTATE_OBJECT = 10;



#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	int		id;
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	DWORD	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	XMFLOAT3 pos;
	XMFLOAT3 look;
	unsigned	move_time;
};

struct CS_CHAT_PACKET {
	unsigned char size;
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_TELEPORT_PACKET {
	unsigned char size;
	char	type;
};

struct CS_LOGOUT_PACKET {
	unsigned char size;
	char	type;
};
struct CS_ROTATE_PACKET {
	unsigned char size;
	char type;
	XMFLOAT3 look;
	XMFLOAT3 right;
	
};

struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 right;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		Max_hp;
	XMFLOAT3 pos;
	char	name[NAME_SIZE];
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
	XMFLOAT3 look;
	XMFLOAT3 right;
	unsigned int move_time;
};
struct SC_ROTATE_OBJECT_PACKET {
	unsigned char size;
	char type;
	int id;


};

struct SC_CHAT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	char	mess[CHAT_SIZE];
};


struct SC_LOGIN_FAIL_PACKET {
	unsigned char size;
	char	type;

};

#pragma pack (pop)