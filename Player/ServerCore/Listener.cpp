#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include"Session.h"
Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		delete(acceptEvent);
	}

}

bool Listener::StartAccept(NetAddress netAddress)
{
	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (GIocpCore.Register(this) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, netAddress) == false)
		return false;
	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32 acceptCount = 1;
	for (int32 i = 0; i < acceptCount; ++i)
	{
		AcceptEvent* acceptEvent = new AcceptEvent;
		_acceptEvents.push_back(acceptEvent);

		RegisterAccept(acceptEvent);

	}


	return false;
}

void Listener::closeSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfbyte)
{
	ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	Session* session = new Session;

	acceptEvent->Init();
	acceptEvent->SetSession(session);
	// iocp에서 처리할 수 있도록 등록하는 
	DWORD bytesRecevied = 0;
	if (false == SocketUtils::acceptEx(_socket, session->GetSocket(),
		session->recvbuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		OUT & bytesRecevied, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errcode = WSAGetLastError();
		if (errcode != WSA_IO_PENDING)
		{
			// 진짜 문제발생 
			RegisterAccept(acceptEvent); // 일단 다시 accept 

		}
	}

}

void Listener::ProcessAccept(AcceptEvent* acceptEvent) // 오버랩을 재사용하는 걸 볼수 있다. 
{
	// TODO
	Session* session = acceptEvent->GetSession();
	SOCKADDR_IN sockAddress;
	int32 sizeofSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeofSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}
	session->SetNetAddress(NetAddress(sockAddress));

	cout << " Client Connected " << endl;

	RegisterAccept(acceptEvent);
}
