#include "pch.h"
#include "Server.h"

extern Timer g_Timer;
extern array<Monster, MAX_NPC> Monsters;

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
	serverAddr.sin_port = htons(PORT_NUM);

	if (SOCKET_ERROR == bind(listensocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)))
		cout << "Bind Error" << endl;

	if (SOCKET_ERROR == listen(listensocket, SOMAXCONN))
		cout << " Listen Error " << endl;
	if (listensocket != INVALID_SOCKET) {
		cout << "Listen socket successfully created." << endl;

		// listensocket의 현재 상태 확인
		int optval;
		int optlen = sizeof(optval);
		if (getsockopt(listensocket, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>(&optval), &optlen) == SOCKET_ERROR) {
			cout << "Failed to get listen socket options." << endl;
		}
		else {
			cout << "Listen socket options retrieved successfully." << endl;
			// 여기서 optval을 통해 listensocket의 상태를 확인할 수 있습니다.
		}

		// 포트 확인
		// getsockname 함수를 사용하여 소켓의 로컬 주소를 가져올 수 있습니다.
		sockaddr_in addr;
		int addrlen = sizeof(addr);
		if (getsockname(listensocket, reinterpret_cast<sockaddr*>(&addr), &addrlen) == SOCKET_ERROR) {
			cout << "Failed to get local address of listen socket." << endl;
		}
		else {
			cout << "Listen socket is bound to port " << ntohs(addr.sin_port) << endl;
		}
	}
	else {
		cout << "Failed to create listen socket." << endl;
	}


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
			cout << error_num << " Error " << endl;
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
					clients[c_id]._pos = { 240.0f,10.0f,730.0f };
				else if (c_id == 1)
					clients[c_id]._pos = { 220.0f,10.0f,760.0f };
				else
					clients[c_id]._pos = { 210.0f, 10.0f,710.0f };

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
		case COMP_TYPE::NPC_UPDATE: {

			int r_id = static_cast<int>(key);
			ingameroom[r_id].UpdateNpc();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms),r_id,EVENT_TYPE::EV_NPC_UPDATE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over; 
			break;
		}
		default:
			break;
		}
	}

}

void Server::InitialziedMonster(int room_Id)
{
	cout << " NPC intialize begin " << endl;

	std::random_device rd;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> xpos(-10, 450);
	std::uniform_real_distribution<float> zpos(-641, -521);

	for (int i = 0; i < MAX_NPC; ++i)
	{
		ingameroom[room_Id].NightMonster[i]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));
		ingameroom[room_Id].NightMonster[i]._att = 10;
		ingameroom[room_Id].NightMonster[i]._hp = 50;
		ingameroom[room_Id].NightMonster[i]._look = XMFLOAT3(0.f, 0.f, 1.0f);
		ingameroom[room_Id].NightMonster[i]._right = XMFLOAT3(1.0f, 0.f, 0.0f);
		ingameroom[room_Id].NightMonster[i]._up = XMFLOAT3(0.f, 1.0f, 0.0f);
		ingameroom[room_Id].NightMonster[i]._is_alive = true;
	}

	cout << " NPC intialzie end " << endl;
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

		while (clients[id].room_id == -1)
		{
			cout << "Matching" << endl;
			//로그인을 해놓고 룸매칭을 하게 해야
		}

		clients[id].send_login_info_packet();
		// ADD X 
	}
				 break;
	case CS_MOVE: {

		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		int r_id = p->roomid;
		clients[id]._pos = p->pos;
		// view List 
		unordered_set<int> near_list;
		clients[id]._v_lock.lock();
		unordered_set<int> old_vlist = clients[id]._view_list;

		clients[id]._v_lock.unlock();

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state == STATE::Alloc || pl->_state == STATE::Free) continue;
			if (pl->_id == id)continue;
			if (pl->_stage != clients[id]._stage)continue;
			if (can_see(id, pl->_id))
				near_list.insert(pl->_id);
		}
		// -------------------------------
		clients[id].send_move_packet(id);

		// -------------------view list 
		for (auto& pl : near_list)
		{
			clients[pl]._v_lock.lock();

			if (clients[pl]._view_list.count(id))
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_move_packet(id);
			}
			else
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_add_info_packet(id);
			}

			if (old_vlist.count(pl) == 0)
				clients[id].send_add_info_packet(pl);
		}



	}
				break;
	case CS_ROTATE: {
		CS_ROTATE_PACKET* p = reinterpret_cast<CS_ROTATE_PACKET*>(packet);
		int r_id = p->roomid;

		clients[id]._look = p->look;
		clients[id]._right = p->right;
		clients[id]._up = p->up;

		unordered_set<int> near_list;
		clients[id]._v_lock.lock();
		unordered_set<int> old_vlist = clients[id]._view_list;
		clients[id]._v_lock.unlock();

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state == STATE::Alloc || pl->_state == STATE::Free) continue;
			if (pl->_id == id)continue;
			if (pl->_stage != clients[id]._stage)continue;
			if (can_see(id, pl->_id))
				near_list.insert(pl->_id);
		}

		clients[id].send_rotate_packet(id);

		for (auto& pl : near_list)
		{

			clients[pl]._v_lock.lock();

			if (clients[pl]._view_list.count(id))
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_rotate_packet(id);
			}
			else
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_add_info_packet(id);
			}
			if (old_vlist.count(pl) == 0)
				clients[id].send_add_info_packet(pl);
		}
	
		break;
	}
	case CS_CHANGE_ANIMATION: {

		CS_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<CS_CHANGE_ANIMATION_PACKET*>(packet);

		int r_id = p->roomid;

		clients[id].animationstate = (animateState)p->a_state;
		clients[id].prevanimationstate = (animateState)p->prev_a_state;

		unordered_set<int> near_list;
		clients[id]._v_lock.lock();
		unordered_set<int> old_vlist = clients[id]._view_list;
		clients[id]._v_lock.unlock();

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state == STATE::Alloc || pl->_state == STATE::Free) continue;
			if (pl->_id == id)continue;
			if (pl->_stage != clients[id]._stage)continue;
			if (can_see(id, pl->_id))
				near_list.insert(pl->_id);
		}

		clients[id].send_change_animate_packet(id);

		for (auto& pl : near_list)
		{

			clients[pl]._v_lock.lock();

			if (clients[pl]._view_list.count(id))
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_change_animate_packet(id);
			}
			else
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_add_info_packet(id);
			}
			if (old_vlist.count(pl) == 0)
				clients[id].send_add_info_packet(pl);
		}
	}
							break;
	case CS_CHANGE_SCENE: {
		cout << " Change scenen" << endl;
		CS_CHANGE_SCENE_PACKET* p = reinterpret_cast<CS_CHANGE_SCENE_PACKET*>(packet);
		int scenenum = p->scenenum;
		int r_id = p->roomid;
		clients[id]._stage = scenenum;
		clients[id].send_change_scene(id, scenenum);

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_id == id)continue;
			pl->send_change_scene(id, clients[id]._stage);
		}
		// 3번 맵 얼음
		if (p->scenenum == 3)
		{
			 // 얼음몬스터 뿌려줘야함 
		}
		// 4번 맵 불 
		else if (p->scenenum == 4)
		{
			// 불몬스터 뿌려줘야함 
		}
		// 5번 맵 자연 
		else if (p->scenenum == 5)
		{
			// 자연몬스터 뿌려줘야함 
		}

	}
						break;
	case CS_INGAME_START: {

		cout << " Game START " << endl;
		CS_INGAME_START_PACKET* p = reinterpret_cast<CS_INGAME_START_PACKET*>(packet);
		int r_id = p->roomid;

		{
			lock_guard<mutex>ll{ clients[id]._s_lock };
			clients[id]._state = STATE::Start;
			ingameroom[r_id].readycnt++;
		}


		if (ingameroom[r_id].readycnt == 3)
		{
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				pl->send_ingame_start();
			}

			InitialziedMonster(r_id);
			ingameroom[r_id].start_time = chrono::system_clock::now();
			TIMER_EVENT ev{ ingameroom[r_id].start_time + chrono::seconds(10s),r_id,EVENT_TYPE::EV_NPC_UPDATE };
			g_Timer.InitTimerQueue(ev);
		}


	}
						break;
	case CS_TIME_CHECK: {

		//CS_TIME_CHECK_PACKET* p = reinterpret_cast<CS_TIME_CHECK_PACKET*>(packet);
		//cout << p->roomid << " 번 방 " << p->time << " 분 경과 " << endl;
		//int r_id = p->roomid;

		//if (p->time % 2 == 0)
		//{
		//	cout << " 몬스터 생성  " << endl;
		//	// 모든 클라이언트들 한테 밤에나오는 NPC들 정보들을 모두 보내줘야 함 
		//	InitialziedMonster(r_id);
		//	for (auto& pl : ingameroom[r_id].ingamePlayer)
		//	{
		//		for (int i = 0; i < ingameroom[r_id].NightMonster.max_size(); ++i) //
		//		{

		//		}
		//	}
		//}
		break;
	};
	
	case CS_ATTACK: {
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		int r_id = p->roomid;

		clients[id]._isAttack = p->isAttack;

		unordered_set<int> near_list;
		clients[id]._v_lock.lock();
		unordered_set<int> old_vlist = clients[id]._view_list;
		clients[id]._v_lock.unlock();

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state == STATE::Alloc || pl->_state == STATE::Free) continue;
			if (pl->_id == id)continue;
			if (pl->_stage != clients[id]._stage)continue;
			if (can_see(id, pl->_id))
				near_list.insert(pl->_id);
		}

		clients[id].send_attack_packet(id);

		for (auto& pl : near_list)
		{

			clients[pl]._v_lock.lock();

			if (clients[pl]._view_list.count(id))
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_attack_packet(id);
			}
			else
			{
				clients[pl]._v_lock.unlock();
				clients[pl].send_add_info_packet(id);
			}
			if (old_vlist.count(pl) == 0)
				clients[id].send_add_info_packet(pl);
		}

	}
				  break;
	case CS_ATTACK_COLLISION:
	{
		CS_ATTACK_COLLISION_PACKET* p = reinterpret_cast<CS_ATTACK_COLLISION_PACKET*>(packet);
		ingameroom[p->room_id].r_l.lock();
		ingameroom[p->room_id].NightMonster[p->npc_id]._is_alive = false;
		ingameroom[p->room_id].r_l.unlock();
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
				lock_guard<mutex> rl{ r_l };
				ingameroom[room_id]._state = roomState::Ingame; // 상태를 Ingame상태로 바꿔준다 
				ingameroom[room_id].ingamePlayer.emplace_back(_session);
				clients[_session->_id].room_id = room_id;

				if (ingameroom[room_id].ingamePlayer.size() == MAX_ROOM_USER)
				{
					ingameroom[room_id].fullcheck = true;

				}
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
		}
		else
			this_thread::sleep_for(1s);

	}
}


