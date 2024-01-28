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

bool IocpCore::Register(SOCKET& _socket) // 소켓 CP에 등록 
{
	// iocp에 등록 
	// 1100 0011
	// 00
	// static > int(long) dynamic reinterpret_cast const_cast
	return ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), _iocpHandle, 9999, 0);

}

void IocpCore::Dispatch(SOCKET s_socket)
{
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_iocpHandle, &num_bytes, &key, &over, INFINITE);
		Over_Exp* ex_over = reinterpret_cast<Over_Exp*>(over);
		if (FALSE == ret)
		{
			if (ex_over->GetType() == COMP_TYPE::Accept) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				//disconnect(static_cast<int>(key));
				//if (ex_over->_comp_type == OP_SEND) delete ex_over;
				//continue;
				
			}
		}
		if ((0 == num_bytes) && ((ex_over->GetType() == COMP_TYPE::Recv) || (ex_over->GetType() == COMP_TYPE::Send)))
		{
			cout << "Disconnect" << endl;
			return;
		}
		switch (ex_over->GetType())
		{
		case COMP_TYPE::Accept: {
			cout << " Accept " << endl;
			int client_id = get_new_client_id();
			if (client_id != -1) {


				{
					lock_guard<Mutex> ll(clients[client_id].s_lock);
					clients[client_id]._state = STATE::Alloc;
				}
				clients[client_id]._pos = { 10.0f,0.0f,0.0f };
				clients[client_id]._id = client_id;
				clients[client_id]._name[0] = 0;
				clients[client_id]._prevremain = 0;
				clients[client_id]._socket = s_socket;
				if (false == Register(s_socket))cout << " accept register error" << endl;
				clients[client_id].do_recv();
				s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else
			{
				cout << "MaxUser " << endl;
			}
			//ZeroMemory();
			//int addr_size = sizeof(SOCKADDR_IN);
			//AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);
			//break;
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


