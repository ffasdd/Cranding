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

	lobbythread = thread([this]() {ReadyToStart(); });

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
				if (c_id == 0)
					clients[c_id]._pos = { 0.0f,0.0f,0.0f };
				else
					clients[c_id]._pos = { 10.0f,0.0f,10.0f };
				clients[c_id]._id = c_id;
				clients[c_id]._name[0] = 0;
				clients[c_id]._prevremain = 0;
				clients[c_id]._socket = clientsocket;
				clients[c_id]._look = XMFLOAT3(0.0f, 0.0f, 1.0f);
				clients[c_id]._up = XMFLOAT3(0.0f, 1.0f, 0.0f);
				clients[c_id]._right = XMFLOAT3(1.0f, 0.0f, 0.0f);
				clients[c_id].characterType = 0;
				clients[c_id].animationstate = animateState::FREE;

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
			//cout << " client send to server " << endl;
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
	case CS_READY_GAME: {
		CS_READY_PACKET* p = reinterpret_cast<CS_READY_PACKET*>(packet);
		clients[id].isReady = true;
	}
					  break;
	case CS_LOGIN: {

		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[id]._name, p->name);
		clients[id].characterType = p->charactertype;
		{
			lock_guard<mutex>ll{ clients[id]._s_lock };
			clients[id]._state = STATE::Ingame;
		}
		matchingqueue.push(&clients[id]);
		while (clients[id].room_id == -1);
		clients[id].send_login_info_packet();
		// ADD X 
	}
				 break;
	case CS_MOVE: {
		// 속한 룸넘버도 같이 넘겨줘야 할듯 
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		int r_id = p->roomid;
		ingameroom[r_id].ingamePlayer[id]->_pos = p->pos;


		// view List 
		unordered_set<int> near_list;
		ingameroom[r_id].ingamePlayer[id]->_v_lock.lock();
		unordered_set<int> old_vlist = ingameroom[r_id].ingamePlayer[id]->_view_list;
		ingameroom[r_id].ingamePlayer[id]->_v_lock.unlock();

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state != STATE::Ingame) continue;
			if (pl->_id == id)continue;
			if (pl->_stage != ingameroom[r_id].ingamePlayer[id]->_stage)continue;
			if (can_see(id, pl->_id))
				near_list.insert(pl->_id);
		}
		// -------------------------------

		ingameroom[r_id].ingamePlayer[id]->send_move_packet(id, ingameroom[r_id].ingamePlayer[id]->_pos);
		// -------------------view list 
		for (auto& pl : near_list)
		{
			auto& cpl = ingameroom[r_id].ingamePlayer[pl];
			cpl->_v_lock.lock();
			if (ingameroom[r_id].ingamePlayer[pl]->_view_list.count(id))
			{
				cpl->_v_lock.unlock();
				ingameroom[r_id].ingamePlayer[pl]->send_move_packet(id, ingameroom[r_id].ingamePlayer[id]->_pos);
			}
			else
			{
				cpl->_v_lock.unlock();
				ingameroom[r_id].ingamePlayer[pl]->send_add_info_packet(id);
			}
			if (old_vlist.count(pl) == 0)
				ingameroom[r_id].ingamePlayer[id]->send_add_info_packet(pl);
		}

		for (auto& pl : old_vlist)
			if (0 == near_list.count(pl)) {
				ingameroom[r_id].ingamePlayer[id]->send_remove_packet(pl);
				ingameroom[r_id].ingamePlayer[pl]->send_remove_packet(id);
			}
		// ------------------------------------------
	}
				break;
	case CS_ROTATE: {
		CS_ROTATE_PACKET* p = reinterpret_cast<CS_ROTATE_PACKET*>(packet);
		int r_id = p->roomid;
		ingameroom[r_id].ingamePlayer[id]->_look = p->look;
		ingameroom[r_id].ingamePlayer[id]->_right = p->right;
		ingameroom[r_id].ingamePlayer[id]->_up = p->up;
		ingameroom[r_id].ingamePlayer[id]->send_rotate_packet(id, p->look, p->right, p->up);
		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_id == id) continue;
			if (pl->_stage != ingameroom[r_id].ingamePlayer[id]->_stage)continue;
			pl->send_rotate_packet(id, ingameroom[r_id].ingamePlayer[id]->_look, ingameroom[r_id].ingamePlayer[id]->_right,
				ingameroom[r_id].ingamePlayer[id]->_up);
		}
		break;
	}
	case CS_CHANGE_ANIMATION: {

		CS_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<CS_CHANGE_ANIMATION_PACKET*>(packet);
		int r_id = p->roomid;
		ingameroom[r_id].ingamePlayer[id]->animationstate = (animateState)p->a_state;
		ingameroom[r_id].ingamePlayer[id]->prevanimationstate = (animateState)p->prev_a_state;
		ingameroom[r_id].ingamePlayer[id]->send_change_animate_packet(id, ingameroom[r_id].ingamePlayer[id]->animationstate, ingameroom[r_id].ingamePlayer[id]->prevanimationstate);
		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_id == id)continue;
			if (pl->_stage != ingameroom[r_id].ingamePlayer[id]->_stage)continue;
			pl->send_change_animate_packet(id, ingameroom[r_id].ingamePlayer[id]->animationstate, ingameroom[r_id].ingamePlayer[id]->prevanimationstate);
		}
	}
							break;
	case CS_CHANGE_SCENE: {
		cout << " Change scenen" << endl;
		CS_CHANGE_SCENE_PACKET* p = reinterpret_cast<CS_CHANGE_SCENE_PACKET*>(packet);
		int scenenum = p->scenenum;
		int r_id = p->roomid;
		ingameroom[r_id].ingamePlayer[id]->_stage = scenenum;
		ingameroom[r_id].ingamePlayer[id]->send_change_scene(id, scenenum);

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_id == id)continue;
			pl->send_change_scene(id, ingameroom[r_id].ingamePlayer[id]->_stage);
		}
	}
						break;
	case CS_ATTACK: {
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		int r_id = p->roomid;
		ingameroom[r_id].ingamePlayer[id]->_isAttack = p->isAttack;
		ingameroom[r_id].ingamePlayer[id]->send_attack_packet(id, ingameroom[r_id].ingamePlayer[id]->_isAttack);
		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{

			if (pl->_id == id)continue;
			if (pl->_stage != ingameroom[r_id].ingamePlayer[id]->_stage)continue;
			pl->send_attack_packet(id, ingameroom[r_id].ingamePlayer[id]->_isAttack);
		}
	}
				  break;


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

int Server::get_new_room_id(unordered_map<int, Room>& rooms)
{
	for (int i = 0; i < MAX_ROOM; ++i)
	{
		if (rooms[i]._state == roomState::Free)
		{
			return i;
		}
		else
		{
			if (rooms[i].fullcheck == false)
				return i;
		}
	}
	return -1;
}

void Server::ReadyToStart()
{
	while (true)
	{
		if (!matchingqueue.empty())
		{
			Session* _session = nullptr;
			bool sessionok = matchingqueue.try_pop(_session);

			if (!sessionok) continue;

			int room_id = get_new_room_id(ingameroom); // room ID를 부여받음 ;
			{
				lock_guard<mutex>{r_l};
				ingameroom[room_id]._state = roomState::Ingame; // 상태를 Ingame상태로 바꿔준다 
				ingameroom[room_id].ingamePlayer.emplace_back(_session);
				clients[_session->_id].room_id = room_id;
				if (ingameroom[room_id].ingamePlayer.size() == MAX_ROOM_USER)
					ingameroom[room_id].fullcheck = true;
			}

			/*	readySession.emplace_back(_session);*/
			//size_t readySessionSize = ingameroom[room_id].ingamePlayer.size();
			// 방안에 있는 애들한테 다 add를 보내줘야함 

			for (auto& ingameplayer : ingameroom[room_id].ingamePlayer)
			{
				for (auto& player : ingameroom[room_id].ingamePlayer)
				{
					if (ingameplayer->_id == player->_id)continue;
					player->send_add_info_packet(ingameplayer->_id);
				}
			}

			if (ingameroom[room_id].ingamePlayer.size() == MAX_ROOM_USER)
			{
				for (auto& pc : ingameroom[room_id].ingamePlayer)
				{
					pc->send_game_start(room_id);
				}
			}
		}
		else
			this_thread::sleep_for(1ms);

	}
}
