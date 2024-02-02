#include"stdafx.h"
#include "Iocp.h"
Over_Exp g_a_over;
Iocp GIocp;
array<Session, MAX_USER> clients;

bool Iocp::Register(SOCKET& socket)
{
	return ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), iocphandle, 9999, 0);

}

void Iocp::WorkerThread()
{
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* over = nullptr;
	BOOL ret = GetQueuedCompletionStatus(iocphandle, &num_bytes, &key, &over, INFINITE);
	Over_Exp* ex_over = reinterpret_cast<Over_Exp*>(over);
	if (FALSE == ret)
	{
		if (ex_over->_type == COMP_TYPE::Accept) cout << "Accept Error";
		else {
			cout << "GQCS Error on client[" << key << "]\n";
			//disconnect(static_cast<int>(key));
			//if (ex_over->_comp_type == OP_SEND) delete ex_over;
			//continue;

		}
	}
	if ((0 == num_bytes) && ((ex_over->_type == COMP_TYPE::Recv) || (ex_over->_type == COMP_TYPE::Send)))
	{
		cout << "Disconnect" << endl;
		return;
	}
	switch (ex_over->_type)
	{
	case COMP_TYPE::Accept: {
		break;
	}
	case COMP_TYPE::Recv: {
		break;
	}
	case COMP_TYPE::Send: {
		break;
	}

	}
}

void Iocp::SetServerSocket(SOCKET serverSocket)
{
	s_socket = serverSocket;
}

void Iocp::SetClientSocket(SOCKET clientSocket)
{
	c_socket = clientSocket;
}

SOCKET Iocp::GetServerSocket()
{
	return s_socket;
}

SOCKET Iocp::GetClientSocket()
{
	return c_socket;
}

int Iocp::makeClientId()
{
	//for (int i = 0; i < MAX_USER; ++i)
	//{
	//	lock_guard<mutex>ll(clients[i].s_lock);
	//	if (clients[i]._state == STATE::Free)
	//		return i;
	//}
}
