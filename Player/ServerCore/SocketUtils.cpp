#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX		SocketUtils::connectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::acceptEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), OUT & wsaData);

	// 런타임에 주소를 얻어오는 API
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&connectEx)));
	ASSERT_CRASH(BindWindowFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&acceptEx)));
	Close(dummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

bool SocketUtils::BindWindowFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	// 런타임에 위에 connectex,acceptex,disconnectex의 주소를 얻어오는 함수 
	DWORD byte = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & byte, NULL, NULL);

}

SOCKET SocketUtils::CreateSocket()
{
	//::socket()
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR !=::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

void SocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}
