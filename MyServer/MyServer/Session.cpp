#include "Session.h"

bool Session::Acceptex(SOCKET listensocket)
{
	_clientsocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_clientsocket == INVALID_SOCKET)
	{
		cout << " Clientsocket Error" << endl;
		return false;
	}

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	ZeroMemory(&_overlapped, sizeof(_overlapped));
	_overlapped._opstate = OPSTATE::ACCEPT;
	if (AcceptEx(listensocket, _clientsocket, _overlapped._sendbuf, 0, addr_size + 16, addr_size + 16, 0, &_overlapped._over) == FALSE)
	{
		int err = WSAGetLastError();
		if (err != ERROR_IO_PENDING) {
			cout << " Error " << endl;
			//return false;
		}
	}


	return true;
}
