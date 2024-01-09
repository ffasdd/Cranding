#include "stdafx.h"
#include "network.h"

void network::WSAinitialize()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cerr << "wsaStartUp Error\n";
		WSACleanup();
		exit(-1);
	}
}

void network::Start()
{
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT_NUM);
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	bind(_s_socket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	listen(_s_socket, SOMAXCONN);

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_s_socket), h_iocp, 9999, 0);
	Accept();

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();

	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back([&]() {WorkerThread(); });

	for (auto& th : worker_threads)
		th.join();

	closesocket(_s_socket);
	WSACleanup();
}

void network::Accept()
{
	int addr_size = sizeof(SOCKADDR_IN);
	_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_over._comptype = OP_ACCEPT;
	AcceptEx(_s_socket, _c_socket, acceptbuf, 0, addr_size + 16, addr_size + 16, 0, &_over._over);

}

void network::WorkerThread()
{
	while (true)
	{
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		Over_Exp* ex_over = reinterpret_cast<Over_Exp*>(over);
		if (FALSE == ret) {
			if (ex_over->_comptype == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				/*disconnect(static_cast<int>(key));*/
				if (ex_over->_comptype == OP_SEND) delete ex_over;
				continue;
			}
		}
		if ((0 == num_bytes) && ((ex_over->_comptype == OP_RECV) || (ex_over->_comptype == OP_SEND))) {
			//disconnect(static_cast<int>(key));
			if (ex_over->_comptype == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comptype)
		{
		case OP_ACCEPT: {
			int client_id = SetId();
			if (client_id != -1)
			{

			{
				lock_guard<mutex>ll{ _clients[client_id]._s_lock };
				_clients[client_id].setState(ST_ALLOC);
			}
			_clients[client_id].setPos(0,0,0);
			_clients[client_id].setId(client_id);
			_clients[client_id].setRemain(0);
			_clients[client_id].setSocket(_c_socket);
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(_c_socket), h_iocp, client_id, 0);
			_clients[client_id].do_recv();
			_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else
			{
				cout << " MAX USER " << endl;
			}
			ZeroMemory(&_over._over, sizeof(_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(_s_socket, _c_socket, _over._sendbuf, 0, addr_size + 16, addr_size + 16, 0, &_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + _clients[key].getPrev_remain();
			char* p = ex_over->_sendbuf;
			while (remain_data > 0)
			{
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					//process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			_clients[key].setPrev_remain(remain_data);
			if (remain_data > 0) {
				memcpy(ex_over->_sendbuf, p, remain_data);
			}
			_clients[key].do_recv();
			break;

		}
		case OP_SEND: {
			delete ex_over;
			break;
		}

		}
	}
}

int network::SetId()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		lock_guard<mutex>ll{ _clients[i]._s_lock };
		if (_clients[i].getState() == ST_FREE)
			return i;
	}
	return  -1;
}
