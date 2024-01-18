#pragma once
#include "NetAddress.h"
class SocketUtils
{
public:
	// 함수의 포인터 같은 애들

	static LPFN_CONNECTEX		connectEx;
	static LPFN_ACCEPTEX		acceptEx;
	static LPFN_DISCONNECTEX	DisconnectEx;

public:
	static void Init();
	static void Clear();
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool BindWindowFunction(SOCKET socket, GUID guid, LPVOID* fn);
	// 소켓을 만든다.
	static SOCKET CreateSocket();

	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Bind(SOCKET socket, NetAddress netAddr);
	static void Close(SOCKET& socket);

};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}