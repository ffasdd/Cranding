#pragma once
#include"IocpCore.h"
#include"IocpEvent.h"
#include"NetAddress.h"

class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();

public:
	// 정보 관련
	void SetNetAddress(NetAddress address) { _netaddress = address; }
	NetAddress GetAddress() { return _netaddress; }
	SOCKET GetSocket() { return _clientsocket; }
	char recvbuffer[100];
public:
	// 인터페이스 구현 
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numofBytes = 0) override; // WorkerThread 
private:

	SOCKET _clientsocket = INVALID_SOCKET;
	NetAddress _netaddress = {};
	Atomic<bool> _connected = false;
};

