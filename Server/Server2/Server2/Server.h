#pragma once
#include"Over_Exp.h"
#include"Session.h"
#include"Room.h"

class Server
{
public:
	static Server& GetInstance();

	Server();
	Server(const Server& server) = delete;
	Server& operator=(const Server& server) = delete;

	~Server();

	void Run();
	void Stop();
	
	void NetworkSet();
	void Iocp();
	void WorkerThread();

	void ProcessPacket(int id, char* packet);

	void disconnect(int id);

	bool can_see(int to, int from);
	int get_new_client_id();
	void ReadyToStart();

public:
	SOCKET listensocket;
	SOCKET clientsocket;

	Over_Exp _overlapped;
	HANDLE _IocpHandle;

	vector<int> lobbyClients;
	vector<thread> worker_thread;
};

