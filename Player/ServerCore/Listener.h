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
	// 외부사용 
	bool StartAccept(NetAddress netAddress);
	void closeSocket();

public:
	// 인터페이스 구현 
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfbyte = 0) override;

private:
	// 수신 관련코드 
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* accemtEvent);

protected:
	
	SOCKET _socket = INVALID_SOCKET;
	vector<AcceptEvent*> _acceptEvents;
};

