#pragma once
#include"IocpCore.h"
#include "NetAddress.h"


class AcceptEvent;

class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	// �ܺλ�� 
	bool StartAccept(NetAddress netAddress);
	void closeSocket();

public:
	// �������̽� ���� 
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfbyte = 0) override;

private:
	// ���� �����ڵ� 
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* accemtEvent);

protected:
	
	SOCKET _socket = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
};

