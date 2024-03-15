#pragma once
#include<thread>
#include<WS2tcpip.h>
#include"Session.h"
#include"../Server/Server2/Server2/protocol.h"
#pragma comment(lib,"WS2_32.lib")

#define SERVER_IP "172.0.0.1"


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

	void ProcessData(int _size);
	void ProcessPacket(char* buf);

public:
	int prev_remain = 0;
private:
	SOCKET clientsocket;
	char _buf[BUF_SIZE];

	thread netThread;

private:

	bool ServerStart = false;

};

