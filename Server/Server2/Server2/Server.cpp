#include "pch.h"
#include "Server.h"
Server& Server::GetInstance()
{
	static Server instance;
	return instance;
}

Server::Server()
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Server::~Server()
{
	CloseHandle(_IocpHandle);
	closesocket(clientsocket);
	closesocket(listensocket);
	WSACleanup();
}

void Server::Run()
{
	NetworkSet();
	Iocp();
}

void Server::Stop()
{

}

void Server::NetworkSet()
{
	listensocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(9000);

	bind(listensocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	listen(listensocket, SOMAXCONN);
}

void Server::Iocp()
{
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listensocket), _IocpHandle, 9999, 0);

	clientsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_overlapped._comptype = COMP_TYPE::Accept;

	BOOL ret = AcceptEx(listensocket, clientsocket, _overlapped._sendbuf, 0, addr_size + 16, addr_size + 16, 0, &_overlapped._over);
	if (FALSE == ret)
	{
		int error_num = WSAGetLastError();
		if (ERROR_IO_PENDING != error_num)
			cout << " Error " << endl;
	}
	int num_thread = std::thread::hardware_concurrency();
	for (int i = 0; i < num_thread; ++i)
		worker_thread.emplace_back(&Server::WorkerThread, this);
	for (auto& th : worker_thread)
		th.join();
}

void Server::WorkerThread()
{
	while (true)
	{
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_IocpHandle, &num_bytes, &key, &over, INFINITE);
		Over_Exp* ex_over = reinterpret_cast<Over_Exp*>(over);

		if (FALSE == ret) {
			if (ex_over->_comptype == COMP_TYPE::Accept) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comptype == COMP_TYPE::Send) delete ex_over;
				continue;
			}
		}
		if ((0 == num_bytes) && ((ex_over->_comptype == COMP_TYPE::Recv) || (ex_over->_comptype == COMP_TYPE::Send))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comptype == COMP_TYPE::Send) delete ex_over;
			continue;
		}

		switch (ex_over->_comptype)
		{
		case COMP_TYPE::Accept: {
			int c_id = get_new_client_id();
			if (c_id != -1) {

				{
					lock_guard<mutex>ll(clients[c_id]._s_lock);
					clients[c_id]._state = STATE::Alloc;
				}
				clients[c_id]._pos.x = 0.0f;
				clients[c_id]._pos.y = 0.0f;
				clients[c_id]._pos.z = 0.0f;
				clients[c_id]._id = c_id;
				clients[c_id]._name[0] = 0;
				clients[c_id]._prevremain = 0;
				clients[c_id]._socket = clientsocket;
				clients[c_id]._look = XMFLOAT3(0.0f, 0.0f, 1.0f);
				clients[c_id]._right = XMFLOAT3(1.0f, 0.0f, 0.0f);

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientsocket), _IocpHandle, c_id, 0);
				clients[c_id].do_recv();
				clientsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else
			{
				cout << " MAX user exceeded" << endl;
			}

			cout << " Accept " << endl;
			ZeroMemory(&_overlapped._over, sizeof(_overlapped._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(listensocket, clientsocket, _overlapped._sendbuf, 0, addr_size + 16, addr_size + 16, 0, &_overlapped._over);
			break;
		}
		case COMP_TYPE::Recv: {
			cout << " client send to server " << endl;
			int remain_data = num_bytes + clients[key]._prevremain;
			char* p = ex_over->_sendbuf;
			while (remain_data > 0)
			{
				int packetsize = p[0];
				if (packetsize <= remain_data) {
					ProcessPacket(static_cast<int>(key), p);
					p = p + packetsize;
					remain_data = remain_data - packetsize;
				}
				else break;
			}
			clients[key]._prevremain = remain_data;
			if (remain_data > 0)
				memcpy(ex_over->_sendbuf, p, remain_data);
			clients[key].do_recv();
			break;
		}
		case COMP_TYPE::Send: {
			delete ex_over;
			break;
		}
		default:
			break;
		}
	}

}

void Server::ProcessPacket(int id, char* packet)
{
	switch (packet[1])
	{
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[id]._name, p->name);
		{
			lock_guard<mutex>ll{ clients[id]._s_lock };
			clients[id]._state = STATE::Ingame;
		}

		clients[id].send_login_info_packet();

		for (auto& pl : clients)
		{
			{
				lock_guard<mutex> ll(pl._s_lock);
				if (STATE::Ingame != pl._state) continue;
			}
			if (pl._id == id)continue;
			if (can_see(id, pl._id) == false)
				continue;
			pl.send_add_info_packet(id);
			clients[id].send_add_info_packet(pl._id);
		}
		break;
	}
	case CS_MOVE: {

		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		clients[id]._pos = p->pos;

		unordered_set<int> near_list;
		clients[id]._v_lock.lock();
		unordered_set<int> old_vlist = clients[id]._view_list;
		clients[id]._v_lock.unlock();

		for (auto& pl : clients)
		{
			if (pl._state != STATE::Ingame) continue;
			if (pl._id == id)continue;
			if (can_see(id, pl._id))
				near_list.insert(pl._id);
		}
		clients[id].send_move_packet(id);

		for (auto& pl : near_list)
		{
			auto& cpl = clients[pl];
			cpl._v_lock.lock();
			if (clients[pl]._view_list.count(id))
			{
				cpl._v_lock.unlock();
				clients[pl].send_move_packet(id);
			}
			else
			{
				cpl._v_lock.unlock();
				clients[pl].send_add_info_packet(id);
			}
			if (old_vlist.count(pl) == 0)
				clients[id].send_add_info_packet(pl);
		}

		for (auto& pl : old_vlist)
			if (0 == near_list.count(pl)) {
				clients[id].send_remove_packet(pl);
				clients[pl].send_remove_packet(id);
			}

	}
				break;
	case CS_ROTATE: {

		CS_ROTATE_PACKET* p = reinterpret_cast<CS_ROTATE_PACKET*>(packet);
		clients[id]._look = p->look;
		clients[id]._right = p->right;
		clients[id]._up = p->up;
		clients[id].send_rotate_packet(id);
		for (auto& pl : clients)
		{
			if (pl._id == id) continue;
			pl.send_rotate_packet(id);
		}
		break;
	}
	}
}

void Server::disconnect(int id)
{
	for (auto& pl : clients)
	{
		if (pl._id == id) continue;
		pl.send_remove_packet(id);
	}
	closesocket(clients[id]._socket);

	lock_guard<mutex>ll(clients[id]._s_lock);
}

bool Server::can_see(int to, int from)
{

	if (abs(clients[from]._pos.x - clients[to]._pos.x) > VIEW_RANGE) return false;
	return abs(clients[from]._pos.z - clients[to]._pos.z) <= VIEW_RANGE;

}

int Server::get_new_client_id()
{

	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[i]._s_lock };
		if (clients[i]._state == STATE::Free)
			return i;
	}
	return -1;

}
