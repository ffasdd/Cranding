#pragma once
#include "define.h"

enum class STATE { FREE, ALLOC, INGAME };
class Session
{
public:
	Session() {}
	~Session() {}

	int _id = 0;

	HANDLE _iocpHandle = INVALID_HANDLE_VALUE;
	INT64 _isConnect = 0;
	SOCKET _clientsocket;
	OverEx _overlapped;

	char recvBuf[BUF_SIZE];

	std::mutex Lock;
	std::queue<OverEx*> SendDataqueue;
	STATE _state;


public:
	void do_recv();
	void do_send();
	bool ConnectIo(HANDLE iocpHandle, SOCKET socket);
	bool Acceptex(SOCKET listensocket);
	bool AcceptComplete();

	void close();


};

