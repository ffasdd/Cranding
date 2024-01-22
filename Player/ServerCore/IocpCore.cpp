#include "pch.h"
#include "IocpCore.h"
#include "Over_Exp.h"

IocpCore GIocpCore;

IocpCore::IocpCore() // cp 생성 
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(SOCKET* _socket) // 소켓 CP에 등록 
{
	// iocp에 등록 
	return ::CreateIoCompletionPort(_socket, _iocpHandle,9999, 0);
	 
}

bool IocpCore::Dispatch(uint32 timeoutMs) // Worker Thread? 
{
	// key 값을 클라이언트의 ID로 관리 해보자 

	DWORD numOfbytes = 0;
	ULONG_PTR key;
	IocpObject* iocpObject = nullptr;
	Over_Exp* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfbytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs)) // timeout 시간이 너무지체되면 에러를 뱉는다 .
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


