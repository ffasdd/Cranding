#include "pch.h"
#include "IocpCore.h"
#include"IocpEvent.h"

IocpCore GIocpCore;

IocpCore::IocpCore() // cp ���� 
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObject* iocpObject) // ���� CP�� ��� 
{
	// iocp�� ��� 
	SOCKET socket;
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle,/*Key*/reinterpret_cast<ULONG_PTR>(iocpObject), 0);
	 
}

bool IocpCore::Dispatch(uint32 timeoutMs) // Worker Thread? 
{
	// key ���� Ŭ���̾�Ʈ�� ID�� ���� �غ��� 

	DWORD numOfbytes = 0;
	ULONG_PTR key;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfbytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs)) // timeout �ð��� �ʹ���ü�Ǹ� ������ ��´� .
	{
		iocpObject->Dispatch(iocpEvent, numOfbytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:

			break;
		}
	}

	return true;
}
