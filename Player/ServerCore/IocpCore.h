#pragma once
class IocpObject // SessionÀÇ ¿ªÇÒ 
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class Over_Exp* iocpEvent, int32 numOfbyte = 0) abstract; 
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();


	bool Register(SOCKET* _socket);
	HANDLE GetHandle() { return _iocpHandle; }
	bool Dispatch(uint32 timeoutMs = INFINITE); // WorkerThread 

private:
	HANDLE _iocpHandle;
};

extern IocpCore GIocpCore;
