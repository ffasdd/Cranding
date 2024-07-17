#pragma once
#include<thread>
#include<WS2tcpip.h>

#include"Session.h"
#include"Player.h"
#include"../Server/Server2/Server2/protocol.h"
#pragma comment(lib,"WS2_32.lib")

#define SERVER_IP "121.170.72.178"
extern unordered_map<int, Session> g_clients;
extern unordered_map<int, Session> g_monsters;
extern unordered_map<int, Session> g_ice_monsters;
extern unordered_map<int, Session> g_fire_monsters;
extern unordered_map<int, Session> g_nature_monsters;

extern Session g_IceBossMonster;
extern Session g_FireBossMonster;
extern Session g_NatureBossMonster;
extern concurrency::concurrent_queue<SENDTYPE> g_sendqueue;
extern HANDLE loginevent;
extern HANDLE startevent;
enum class animateState;

class Network
{
public:
	Network();
	~Network();

public:
	bool ReadytoConnect();
	void End();
	
public:
	// thread recv
	void StartServer();
	void NetThreadFunc();

	void ProcessData(size_t _size);
	void ProcessPacket( char* buf);

	SOCKET GetSocket() { return clientsocket; }
	int Getmyid() { return my_id; }
public: // thread send 
	
	void SendThreadFunc();
	void SendProcess(SENDTYPE sendtype);
	void SendLoginfo();
	void SendTest();
	void SendMovePlayer(XMFLOAT3 _pos);
	//void SendRotatePlayer(XMFLOAT3 _look,XMFLOAT3 _right, XMFLOAT3 _up);
	void SendRotatePlayer(float _yaw);
	void SendChangeAnimation(int curanimate, int prevanimate);
	void SendChangeScene(int scenenum);
	void SendIngameStart();
	void SendAttack(bool is_attack);
	void SendAttackCollision(int npc_id, MonsterType _mtype);
	void SendReady();
	void SendTime(int time);
	
	bool MonsterCollide(Session& _monster);
	
public: // utils
	int getmyid(int _id);
	bool IngameScene = false;
	bool IngameStart = false;
	bool gamestart = false;
	bool SpaceshipScene = false;
	bool ClientState = false;
public:
	int prev_remain = 0;
	int my_id = 0;
	int my_roomid = -1;
	int stage_num = -1;
	bool isNight = false;

	
private:
	SOCKET clientsocket;
	 char _buf[BUF_SIZE];

	thread netThread;
	thread sendThread;
	thread timerThread;

	int ingamecnt = 0;

	clock_t start;
	clock_t end;
	int second{ 30 };
	unsigned int curTimer = 0;
	//CPlayer* PlayerInfo;

	float                           m_fBoundingSize{ 3.0f };
	BoundingSphere					m_SPBB = BoundingSphere(XMFLOAT3(0.0f, 0.0f, 0.0f), m_fBoundingSize);

	
private:
	bool ServerStart = false;

};

