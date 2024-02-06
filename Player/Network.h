#pragma once
#include"thread"
#include"Player.h"
#include<thread>
#include"../Server/Server2/Server2/protocol.h"
class Network
{
public:
	static Network& GetInstance();

	Network();
	Network(const Network& network) = delete;
	Network& operator=(const Network& network) = delete;

	~Network();

	void Login();
	void Run();
	void Close();
	void do_send();
	void do_recv();

	void processData(char* buf, size_t recv_num);
	void processPacket(char* buf);

private:
	SOCKET clientSocket;
	char sendbuf[100];
	WSABUF wsaBuf;
	int my_id;


};
extern HANDLE g_event;
extern array<CPlayer, 3> clients;
