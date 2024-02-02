#pragma once
#include"Session.h"
#include"Over_Exp.h"
extern array<Session, MAX_USER> clients;

class Iocp
{
public:
	Iocp() {

	}
	Iocp(Over_Exp a_over) : overlapped{ a_over } 
	{
		iocphandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	}
	~Iocp() {}

public:
	bool Register(SOCKET& socket);
	void WorkerThread();
	void SetServerSocket(SOCKET serverSocket);
	void SetClientSocket(SOCKET clientSocket);
	SOCKET GetServerSocket();
	SOCKET GetClientSocket();
	int makeClientId();

private:
	Over_Exp overlapped = nullptr;
	HANDLE iocphandle = nullptr;
	SOCKET s_socket = 0;
	SOCKET c_socket = 0;
};
extern Iocp GIocp;


