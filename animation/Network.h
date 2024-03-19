#pragma once
#include<thread>
#include<WS2tcpip.h>
#include"Session.h"
#include"../Server/Server2/Server2/protocol.h"
#pragma comment(lib,"WS2_32.lib")

#define SERVER_IP "172.0.0.1"

extern unordered_map<int, Session> g_clients;
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
	// thread 
	void StartServer();
	void NetThreadFunc();
	void ProcessData(size_t _size);
	void ProcessPacket(char* buf);

	SOCKET GetSocket() { return clientsocket; }
public: // send
	void SendLoginfo(char* name);
	void SendTest();
	void SendMovePlayer(XMFLOAT3 _pos);

public:
	int prev_remain = 0;
private:
	SOCKET clientsocket;
	char _buf[BUF_SIZE];

	thread netThread;

private:

	bool ServerStart = false;

};

