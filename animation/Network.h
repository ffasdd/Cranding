#pragma once
#include<thread>
#include<WS2tcpip.h>

#pragma comment(lib,"WS2_32.lib")

#define SERVER_IP "172.0.0.1"


class Network
{
public:

private:
	SOCKET clientsocket;
	char _buf[BUF_SIZE];

};

