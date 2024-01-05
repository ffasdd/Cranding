#pragma once
#include"Over_Exp.h"
#include"GameObject.h"
class network
{

public:

	HANDLE h_iocp;
	SOCKET _s_socket;
	SOCKET _c_socket;
	
	Over_Exp _over;
	char acceptbuf[BUF_SIZE];

	array<Player, MAX_USER> _clients;

	
public:
	
	// 윈속 초기화 
	void WSAinitialize();
	
	void Start();
	void Accept();
	void WorkerThread();

public:
	HANDLE getIocpHandle() { return h_iocp; }
	int SetId();
};

