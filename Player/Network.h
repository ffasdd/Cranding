#pragma once
#include"thread"
#include"Session.h"
#include<thread>
#include"../Server/Server2/Server2/protocol.h"

enum class COMP_TYPE : int { Accept, Recv, Send };
extern HANDLE g_event;
extern array<Session, 3> clients;
extern queue<int> clientsendque;


class OVER_EX {
public:
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	char send_buf[BUF_SIZE];
	COMP_TYPE process_type;

	OVER_EX()
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = send_buf;
		process_type = COMP_TYPE::Recv;
		ZeroMemory(&overlapped, sizeof(overlapped));
	}

	OVER_EX(char* packet)
	{
		wsabuf.len = packet[0];
		wsabuf.buf = send_buf;
		ZeroMemory(&overlapped, sizeof(overlapped));
		process_type = COMP_TYPE::Send;
		memcpy(send_buf, packet, packet[0]);
	}
};

class Network
{
public:
	static Network& GetInstance();
	Network();
	Network(const Network& network) = delete;
	Network& operator=(const Network& network) = delete;

	~Network();

	void Run();
	
	void Login_send();
	void recvPacket();

	void processData(CHAR* buf, size_t recv_num);
	void processPacket(char* buf);
	void sendprocess(int sendtype);

private:
	SOCKET clientSocket;
	char sendbuf[100];
	char recvbuf[BUF_SIZE];
	WSABUF wsaBuf;
	int my_id;
	OVER_EX recv_over;
};
