#pragma once
#include<thread>
#include<WS2tcpip.h>
#include"Session.h"
#include"Player.h"
#include"../Server/Server2/Server2/protocol.h"
#pragma comment(lib,"WS2_32.lib")

#define SERVER_IP "172.0.0.1"

extern unordered_map<int, Session> g_clients;
extern queue<SENDTYPE> g_sendqueue;
extern HANDLE g_event;

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
	void ProcessPacket(char* buf);

	SOCKET GetSocket() { return clientsocket; }
	int Getmyid() { return my_id; }
public: // thread send 
	
	void SendThreadFunc();
	void SendProcess(SENDTYPE sendtype);
	void SendLoginfo(char* name);
	void SendTest();
	void SendMovePlayer(XMFLOAT3 _pos);
	void SendRotatePlayer(XMFLOAT3 _look,XMFLOAT3 _right, XMFLOAT3 _up);
	void SendChangeAnimation(animateState curanimate, animateState prevanimate);

public:
	int prev_remain = 0;
	int my_id = 0;

private:
	SOCKET clientsocket;
	char _buf[BUF_SIZE];

	thread netThread;
	thread sendThread;

	//CPlayer* PlayerInfo;

private:

	bool ServerStart = false;

};

