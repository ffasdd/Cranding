#pragma once
#include"Session.h"

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

public:
	bool Register(SOCKET& _socket);
	HANDLE GetHandle() { return _iocpHandle; }
	void Dispatch(SOCKET s_socket); // WorkerThread 

	int get_new_client_id()
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			lock_guard<Mutex>ll{ clients[i].s_lock };
			if (clients[i]._state == STATE::Free)
				return i;
		}
	}

private:
	HANDLE _iocpHandle;
};
extern IocpCore GIocpCore;
extern array<Session, MAX_USER> clients;