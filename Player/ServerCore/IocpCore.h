#pragma once
#include"Session.h"
extern array<Session, MAX_USER> clients;

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

public:
	bool Register(SOCKET& _socket);
	HANDLE GetHandle() { return _iocpHandle; }
	void Dispatch(); // WorkerThread 
	SOCKET GetListenSocket() { return s_socket; }
	SOCKET GetServerSocket() { return c_socket; }
	void SetListenSocket(SOCKET socket) { s_socket = socket; }
	void SetServerSocket(SOCKET socket) { c_socket = socket; }
	int get_new_client_id()
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			lock_guard<Mutex>ll( clients[i].s_lock );
			if (clients[i]._state == STATE::Free)
				return i;
		}
	}

private:
	HANDLE _iocpHandle;
	SOCKET s_socket;
	SOCKET c_socket;
};
extern IocpCore GIocpCor