#pragma once
#include "NetAddress.h"
class SocketUtils
{
public:
	// �Լ��� ������ ���� �ֵ�

	static LPFN_CONNECTEX		connectEx;
	static LPFN_ACCEPTEX		acceptEx;
	static LPFN_DISCONNECTEX	DisconnectEx;

public:
	static void Init();
	static void Clear();

	static bool BindWindowFunction(SOCKET socket, GUID guid, LPVOID* fn);
	// ������ �����.
	static SOCKET CreateSocket();

	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Bind(SOCKET socket, NetAddress netAddr);
	static void Close(SOCKET& socket);

};

