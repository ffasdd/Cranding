#pragma once
#include <iostream>
#include<random>
#include<array>
#include<vector>
#include<map>
#include<queue>
#include<set>
#include<unordered_map>
#include<unordered_set>
#include <concurrent_priority_queue.h>
#include<mutex>
#include<thread>
#include<atomic>

using namespace std;


#include <winsock2.h>
#include<MSWSock.h>
#include<ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

constexpr int MAX_CLIENTS = 1000;
constexpr int PORT_NUM = 7777;
constexpr int BUF_SIZE = 256;

enum class OPSTATE
{
	ACCEPT, RECV, SEND 
};

struct OverEx
{
	WSAOVERLAPPED _over;
	WSABUF _wsaBuf;
	char _sendbuf[BUF_SIZE];
	OPSTATE _opstate;
	
	OverEx()
	{

	}
	OverEx(char* packet)
	{

	}
};