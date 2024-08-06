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

	//initialize_DB();
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
	//int _id = -1;
	//int _password = -1;
	//cout << " 학번을 입력하시오 " << endl;
	//cin >> _id;
	//cout << " 비밀번호를 입력하시오 " << endl;
	//cin >> _password;
	//isAllowAccess(_id, _password);
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
	LINGER option;
	option.l_linger = 0;
	option.l_onoff = 1;
	setsockopt(clientsocket, SOL_SOCKET, SO_LINGER, (const char*)&option, sizeof(option));
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
				else if (c_id % 2 == 0)
					clients[c_id]._pos = { 210.0f, 10.0f,710.0f };
				else if (c_id % 2 == 1)
					clients[c_id]._pos = { 240.0f,10.0f,730.0f };

				clients[c_id]._id = c_id;
				clients[c_id]._name[0] = 0;
				clients[c_id]._prevremain = 0;
				clients[c_id]._socket = clientsocket;
				clients[c_id]._look = XMFLOAT3(0.0f, 0.0f, 1.0f);
				clients[c_id]._up = XMFLOAT3(0.0f, 1.0f, 0.0f);
				clients[c_id]._right = XMFLOAT3(1.0f, 0.0f, 0.0f);
				clients[c_id].characterType = 0;
				clients[c_id].animationstate = animateState::FREE;

				clients[c_id].m_SPBB.Center = clients[c_id]._pos;
				clients[c_id].m_SPBB.Center.y = 20.0f;
				clients[c_id].m_SPBB.Radius = 10.0f;

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientsocket), _IocpHandle, c_id, 0);
				clients[c_id].do_recv();
				clientsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else
			{
				cout << " MAX user exceeded" << endl;
			}

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
		case COMP_TYPE::NPC_INITIALIZE: {
			int r_id = static_cast<int>(key);
			InitialziedMonster(r_id);
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::seconds(60s),r_id,EVENT_TYPE::EV_NPC_INITIALIZE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::NIGHT_TIMER: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].NightSend();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::seconds(60s),r_id,EVENT_TYPE::EV_NIGHT };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::DAYTIME_TIMER: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].DayTimeSend();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::seconds(30s),r_id,EVENT_TYPE::EV_DAYTIME };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::ICE_NPC_UPDATE: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].IceUpdateNpc();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms), r_id,EVENT_TYPE::EV_ICE_NPC_UPDATE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::FIRE_NPC_UPDATE: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].FireUpdateNpc();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms), r_id,EVENT_TYPE::EV_FIRE_NPC_UPDATE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::NATURE_NPC_UPDATE: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].NatureUpdateNpc();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms), r_id,EVENT_TYPE::EV_NATURE_NPC_UPDATE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::ICE_BOSS_MOVE: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].IceBossUpdate();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms),r_id,EVENT_TYPE::EV_ICE_BOSS_MOVE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::FIRE_BOSS_MOVE: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].FireBossUpdate();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms),r_id,EVENT_TYPE::EV_FIRE_BOSS_MOVE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::NATURE_BOSS_MOVE: {
			int r_id = static_cast<int>(key);
			ingameroom[r_id].NatureBossUpdate();
			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(20ms),r_id,EVENT_TYPE::EV_NATURE_BOSS_MOVE };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::PLAYER_ATTACKED_NPC: {
			int r_id = static_cast<int>(key);
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				pl->send_player_attack_mosnter(ex_over->_ai_target_obj, false, ex_over->_monstertype);
			}
			delete ex_over;
			break;
		}
		case COMP_TYPE::ICE_BOSS_SKILL: {
			int r_id = static_cast<int>(key);
			if (ingameroom[r_id].IceBoss._is_alive == false)break;
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				if (ingameroom[r_id].IceBoss._fight == false)break;
				if (pl->_stage != 3)continue;
				pl->send_iceboss_skill(true);
				// 스킬 send 
			}

			TIMER_EVENT ev1{ std::chrono::system_clock::now() + std::chrono::milliseconds(2s), r_id , EVENT_TYPE::EV_ICE_BOSS_SKILL_CANCLE };
			g_Timer.InitTimerQueue(ev1);

			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(6s), r_id , EVENT_TYPE::EV_ICE_BOSS_SKILL };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::ICE_BOSS_CANCLE_SKILL: {
			int r_id = static_cast<int>(key);
			if (ingameroom[r_id].IceBoss._is_alive == false)break;
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				if (ingameroom[r_id].IceBoss._fight == false)break;
				if (pl->_stage != 3)continue;
				pl->send_iceboss_skill(false);
			}
			delete ex_over;
			break;
		}
		case COMP_TYPE::FIRE_BOSS_SKILL: {
			int r_id = static_cast<int>(key);
			if (ingameroom[r_id].FireBoss._is_alive == false)break;
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				if (ingameroom[r_id].FireBoss._fight == false)break;
				if (pl->_stage != 4)continue;
				pl->send_fireboss_skill(true);
				// 스킬 send 
			}

			TIMER_EVENT ev1{ std::chrono::system_clock::now() + std::chrono::milliseconds(2s), r_id , EVENT_TYPE::EV_FIRE_BOSS_SKILL_CANCLE };
			g_Timer.InitTimerQueue(ev1);

			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(6s), r_id , EVENT_TYPE::EV_FIRE_BOSS_SKILL };
			g_Timer.InitTimerQueue(ev);

			delete ex_over;
			break;
		}
		case COMP_TYPE::FIRE_BOSS_CANCLE_SKILL: {
			int r_id = static_cast<int>(key);
			if (ingameroom[r_id].FireBoss._is_alive == false)break;
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				if (ingameroom[r_id].FireBoss._fight == false)break;
				if (pl->_stage != 4)continue;
				pl->send_fireboss_skill(false);
			}
			delete ex_over;
			break;
		}
		case COMP_TYPE::NATURE_BOSS_SKILL: {
			int r_id = static_cast<int>(key);
			if (ingameroom[r_id].NatureBoss._is_alive == false)break;
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				if (ingameroom[r_id].NatureBoss._fight == false)break;
				if (pl->_stage != 5)continue;
				pl->send_natureboss_skill(true);
				// 스킬 send 
			}

			TIMER_EVENT ev1{ std::chrono::system_clock::now() + std::chrono::milliseconds(2s), r_id , EVENT_TYPE::EV_NATURE_BOSS_SKILL_CANCLE };
			g_Timer.InitTimerQueue(ev1);

			TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(6s), r_id , EVENT_TYPE::EV_NATURE_BOSS_SKILL };
			g_Timer.InitTimerQueue(ev);
			delete ex_over;
			break;
		}
		case COMP_TYPE::NATURE_BOSS_SKILL_CANCLE: {
			int r_id = static_cast<int>(key);
			if (ingameroom[r_id].NatureBoss._is_alive == false)break;
			for (auto& pl : ingameroom[r_id].ingamePlayer)
			{
				if (ingameroom[r_id].NatureBoss._fight == false)break;
				if (pl->_stage != 5)continue;
				pl->send_natureboss_skill(false);
			}
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
	std::random_device rd;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> xpos(20, 360);
	std::uniform_real_distribution<float> zpos(-710, 160);

	std::uniform_real_distribution<float> i_xpos(940, 1400);
	std::uniform_real_distribution<float> i_zpos(1100, 1300);

	std::uniform_real_distribution<float> n_xpos(-900, -500);
	std::uniform_real_distribution<float> n_zpos(1100, 1500);

	for (int i = 0; i < MAX_NPC; ++i)
	{
		if (ingameroom[room_Id].NightMonster[i]._is_alive == false)
		{
			if (i < 10)
			{
				ingameroom[room_Id].NightMonster[i]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));
				ingameroom[room_Id].NightMonster[i]._m_type = MonsterType::Night;
			}
			else if (10 <= i && i < 20)
			{
				ingameroom[room_Id].NightMonster[i]._pos = XMFLOAT3(i_xpos(dre), 10.0f, i_zpos(dre));
				ingameroom[room_Id].NightMonster[i]._m_type = MonsterType::Night;
			}
			else if (20 <= i && i < 30)
			{
				ingameroom[room_Id].NightMonster[i]._pos = XMFLOAT3(n_xpos(dre), 10.0f, n_zpos(dre));
				ingameroom[room_Id].NightMonster[i]._m_type = MonsterType::Night;
			}
			ingameroom[room_Id].NightMonster[i]._id = i;
			ingameroom[room_Id].NightMonster[i]._att = 10;
			ingameroom[room_Id].NightMonster[i]._hp = 50;
			ingameroom[room_Id].NightMonster[i]._look = XMFLOAT3(0.f, 0.f, 1.0f);
			ingameroom[room_Id].NightMonster[i]._right = XMFLOAT3(1.0f, 0.f, 0.0f);
			ingameroom[room_Id].NightMonster[i]._up = XMFLOAT3(0.f, 1.0f, 0.0f);
			ingameroom[room_Id].NightMonster[i]._is_alive = true;
			ingameroom[room_Id].NightMonster[i]._stagenum = 2;
			ingameroom[room_Id].NightMonster[i].m_SPBB.Center = ingameroom[room_Id].NightMonster[i]._pos;
			ingameroom[room_Id].NightMonster[i].m_SPBB.Center.y = 20.0f;
			ingameroom[room_Id].NightMonster[i].m_SPBB.Radius = 8.0f;
			ingameroom[room_Id].NightMonster[i]._spaceship = &ingameroom[room_Id]._spaceship;


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
		clients[id].characterType = p->charactertype;
		{
			lock_guard<mutex>ll{ clients[id]._s_lock };
			clients[id]._state = STATE::Ingame;
			clients[id]._stage = 1;
			clients[id]._attpow = 20;
			clients[id]._hp = 100;
			clients[id]._speed = 70;
		}

		matchingqueue.push(&clients[id]);

		while (clients[id].room_id == -1)
		{
			this_thread::yield();
		}

		clients[id].send_login_info_packet();
	}
				 break;
	case CS_MOVE: {

		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		int r_id = p->roomid;

		clients[id]._pos = p->pos;

		clients[id].m_SPBB.Center = clients[id]._pos;
		clients[id].m_SPBB.Center.y = 20.0f;

		for (auto& pl : clients)
		{
			if (pl._state == STATE::Alloc || pl._state == STATE::Free) continue;
			if (pl._id == id)continue;
			if (pl.room_id != clients[id].room_id)continue;
			if (pl._stage != clients[id]._stage)continue;
			pl.send_move_packet(id); // 다른 클라이언트들 한테 나의 좌표를 보내줌 
		}
	}
				break;
	case CS_ROTATE: {

		CS_ROTATE_PACKET* p = reinterpret_cast<CS_ROTATE_PACKET*>(packet);
		int r_id = p->roomid;
		clients[id].Rotate(p->yaw);
		for (auto& pl : clients)
		{
			if (pl._state == STATE::Alloc || pl._state == STATE::Free) continue;
			if (pl._id == id)continue;
			if (pl.room_id != clients[id].room_id)continue;
			if (pl._stage != clients[id]._stage)continue;
			pl.send_rotate_packet(id,p->yaw);
		}
		break;
	}

	case CS_CHANGE_ANIMATION: {

		CS_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<CS_CHANGE_ANIMATION_PACKET*>(packet);
		int r_id = p->roomid;
		clients[id].animationstate = (animateState)p->a_state;
		clients[id].prevanimationstate = (animateState)p->prev_a_state;

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state == STATE::Alloc || pl->_state == STATE::Free) continue;
			if (pl->_id == id)continue;
			if (pl->_stage != clients[id]._stage) continue;
			pl->send_change_animate_packet(id);
			cout << " Animation" << endl;
		}
	}
							break;
	case CS_CHANGE_SCENE: {

		std::random_device rd;
		std::default_random_engine dre;

		CS_CHANGE_SCENE_PACKET* p = reinterpret_cast<CS_CHANGE_SCENE_PACKET*>(packet);
		int scenenum = p->scenenum;
		int r_id = p->roomid;
		{
			lock_guard<mutex>ll{ clients[id]._s_lock };
			clients[id]._stage = scenenum;
		}
		switch (scenenum)
		{
		case 2: {
			clients[id]._stage = scenenum;
		}
			  break;
		case 3: {

			clients[id]._stage = scenenum;
			std::uniform_real_distribution<float> xpos(-500, -400);
			std::uniform_real_distribution<float> zpos(1120, 1200);
			clients[id]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));

			if (ingameroom[r_id].iceupdate == false)
			{
				ingameroom[r_id].iceupdate = true;

				TIMER_EVENT ev4{ std::chrono::system_clock::now() ,r_id,EVENT_TYPE::EV_ICE_NPC_UPDATE};
				g_Timer.InitTimerQueue(ev4);

				TIMER_EVENT ev7{ std::chrono::system_clock::now(),r_id,EVENT_TYPE::EV_ICE_BOSS_MOVE };
				g_Timer.InitTimerQueue(ev7);
			}

			if (ingameroom[r_id].icebossskill == false)
			{
				ingameroom[r_id].icebossskill = true;
				TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::seconds(7s), r_id,EVENT_TYPE::EV_ICE_BOSS_SKILL };
				g_Timer.InitTimerQueue(ev);
			}
		}
			  break;
		case 4: {
			clients[id]._stage = scenenum;
			std::uniform_real_distribution<float> xpos(-732, -570);
			std::uniform_real_distribution<float> zpos(531, 580);
			clients[id]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));

			if (ingameroom[r_id].fireupdate == false)
			{
				ingameroom[r_id].fireupdate = true;
			TIMER_EVENT ev5{ std::chrono::system_clock::now(),r_id,EVENT_TYPE::EV_FIRE_NPC_UPDATE };
			g_Timer.InitTimerQueue(ev5);

			TIMER_EVENT ev8{ std::chrono::system_clock::now(),r_id,EVENT_TYPE::EV_FIRE_BOSS_MOVE };
			g_Timer.InitTimerQueue(ev8);
			}
			if (ingameroom[r_id].firebossskill == false)
			{
				ingameroom[r_id].firebossskill = true;
				TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::seconds(7s), r_id,EVENT_TYPE::EV_FIRE_BOSS_SKILL };
				g_Timer.InitTimerQueue(ev);
			}
		}
			  break;
		case 5: {
			clients[id]._stage = scenenum;
			std::uniform_real_distribution<float> xpos(-732, -570);
			std::uniform_real_distribution<float> zpos(531, 580);
			clients[id]._pos = XMFLOAT3(xpos(dre), 10.0f, zpos(dre));

			if (ingameroom[r_id].natureupdate)
			{
				ingameroom[r_id].natureupdate = true; 
				TIMER_EVENT ev6{ std::chrono::system_clock::now(),r_id,EVENT_TYPE::EV_NATURE_NPC_UPDATE };
				g_Timer.InitTimerQueue(ev6);

				TIMER_EVENT ev9{ std::chrono::system_clock::now(),r_id,EVENT_TYPE::EV_NATURE_BOSS_MOVE };
				g_Timer.InitTimerQueue(ev9);
			}
			if (ingameroom[r_id].naturebossskill == false)
			{
				ingameroom[r_id].naturebossskill = true;
				TIMER_EVENT ev{ std::chrono::system_clock::now() + std::chrono::seconds(7s), r_id,EVENT_TYPE::EV_NATURE_BOSS_SKILL };
				g_Timer.InitTimerQueue(ev);
			}
		}
			  break;
		}
		//clients[id].send_change_scene(id, scenenum); // 나한테 나의 씬넘버를 보냄 
		// 할필요가없음 
		for (auto& pl : ingameroom[r_id].ingamePlayer) // 나의 씬번호를 다른 플레이어들한테 보냄 
		{
			if (pl->_id == id)continue;
			pl->send_change_scene(id, clients[id]._stage);
		}
	}
						break;
	case CS_INGAME_START: {

		CS_INGAME_START_PACKET* p = reinterpret_cast<CS_INGAME_START_PACKET*>(packet);
		int r_id = p->roomid;
		{
			lock_guard<mutex>ll{ clients[id]._s_lock };
			clients[id]._state = STATE::Start;
		}
		{
			lock_guard<mutex>ll{ ingameroom[r_id].r_l };
			ingameroom[r_id].readycnt++;
		}
		if (ingameroom[r_id].readycnt < MAX_ROOM_USER) break;


		ingameroom[r_id].BossMonsterInitialziedMonster();
		ingameroom[r_id].IceNpcInitialized();
		ingameroom[r_id].FireNpcInitialized();
		ingameroom[r_id].NatureNpcInitialized();

		ingameroom[r_id].start_time = chrono::system_clock::now();

		TIMER_EVENT ev2{ ingameroom[r_id].start_time,r_id,EVENT_TYPE::EV_NIGHT };
		g_Timer.InitTimerQueue(ev2);

		TIMER_EVENT ev3{ ingameroom[r_id].start_time + chrono::seconds(10s),r_id,EVENT_TYPE::EV_DAYTIME };
		g_Timer.InitTimerQueue(ev3);

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			pl->send_ingame_start();
		}
	}
						break;
	case CS_MONSTER_INITIALIZE: {

		CS_MOSNTSER_INITIALIZED_PACKET* p = reinterpret_cast<CS_MOSNTSER_INITIALIZED_PACKET*>(packet);
		int r_id = p->room_id;

		ingameroom[r_id].start_time = chrono::system_clock::now();

		TIMER_EVENT ev1{ ingameroom[r_id].start_time,r_id,EVENT_TYPE::EV_NPC_INITIALIZE };
		g_Timer.InitTimerQueue(ev1);

		TIMER_EVENT ev{ ingameroom[r_id].start_time,r_id,EVENT_TYPE::EV_NPC_UPDATE };
		g_Timer.InitTimerQueue(ev);

		break;

	}
	case CS_ATTACK: {
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		int r_id = p->roomid;

		clients[id]._isAttack = p->isAttack;

		std::chrono::system_clock::time_point attackTime = std::chrono::system_clock::now();

		for (auto& pl : ingameroom[r_id].ingamePlayer)
		{
			if (pl->_state == STATE::Alloc || pl->_state == STATE::Free)continue;
			if (pl->_id == id) continue;
			if (pl->_stage != clients[id]._stage)continue;
			pl->send_attack_packet(id);


		}
	}
				  break;
	case CS_ATTACK_COLLISION:
	{
		CS_ATTACK_COLLISION_PACKET* p = reinterpret_cast<CS_ATTACK_COLLISION_PACKET*>(packet);

		switch (p->_montype)
		{
		case MonsterType::Night:
		{
			ingameroom[p->room_id].NightMonster[p->npc_id].MonsterLock.lock();
			ingameroom[p->room_id].NightMonster[p->npc_id]._is_alive = false;
			ingameroom[p->room_id].NightMonster[p->npc_id].MonsterLock.unlock();

			break;
		}
		case MonsterType::Fire:
		{
			ingameroom[p->room_id].FireMonster[p->npc_id].MonsterLock.lock();
			ingameroom[p->room_id].FireMonster[p->npc_id]._is_alive = false;
			ingameroom[p->room_id].FireMonster[p->npc_id].MonsterLock.unlock();
			break;
		}
		case MonsterType::Ice:
		{
			ingameroom[p->room_id].IceMonster[p->npc_id].MonsterLock.lock();
			ingameroom[p->room_id].IceMonster[p->npc_id]._is_alive = false;
			ingameroom[p->room_id].IceMonster[p->npc_id].MonsterLock.unlock();
			break;
		}
		case MonsterType::Nature:
		{
			ingameroom[p->room_id].NatureMonster[p->npc_id].MonsterLock.lock();
			ingameroom[p->room_id].NatureMonster[p->npc_id]._is_alive = false;
			ingameroom[p->room_id].NatureMonster[p->npc_id].MonsterLock.unlock();
			break;
		}
		case MonsterType::Fire_Boss:
		{
			ingameroom[p->room_id].FireBoss._is_alive = false;
			break;
		}
		case MonsterType::Ice_Boss:
		{
			ingameroom[p->room_id].IceBoss._is_alive = false;
			break;
		}
		case MonsterType::Nature_Boss:
		{
			ingameroom[p->room_id].NatureBoss._is_alive = false;
			break;
		}
		}
	}
	break;
	case CS_MONSTER_DIE: {
		CS_MONSTER_DIE_PACKET* p = reinterpret_cast<CS_MONSTER_DIE_PACKET*>(packet);
		// 다른 플레이어들 한테 NPC 타격 상황을 보냄 
		switch (p->_montype)
		{
		case MonsterType::Night:
		{
			if (p->npc_id >= 0 && p->npc_id < 10)
			{
				clients[p->id]._fireMonstercnt++;
				cout << id << " - ID kill FireMonster " << clients[id]._fireMonstercnt << endl;
			}
			else if (p->npc_id >= 10 && p->npc_id < 20)
			{
				clients[p->id]._iceMontsercnt++;
				cout << id << " - ID kill IceMonster " << clients[id]._iceMontsercnt << endl;
			}
			else
			{
				clients[p->id]._natureMonstercnt++;
				cout << id << " - ID kill NatureMonster " << clients[id]._natureMonstercnt << endl;
			}
		}
		break;
		case MonsterType::Fire:
		{

		}
		break;
		case MonsterType::Ice:
		{

		}
		break;
		case MonsterType::Nature:
		{

		}
		break;

		}

		for (auto& pl : ingameroom[p->room_id].ingamePlayer)
		{
			if (pl->_stage != clients[id]._stage)continue;
			pl->send_player_attack_mosnter(p->npc_id, true, p->_montype);

			std::chrono::system_clock::time_point attacktime = chrono::system_clock::now();
			TIMER_EVENT ev{ attacktime + 1s ,p->room_id,EVENT_TYPE::EV_PLAYER_ATTACK_NPC,p->npc_id,p->_montype };
			g_Timer.InitTimerQueue(ev);
		}
		break;
	}

	case CS_MONSTER_HIT_SPACESHIP: {
		CS_MONSTER_ATTACK_SPACESHIP_PACKET* p = reinterpret_cast<CS_MONSTER_ATTACK_SPACESHIP_PACKET*>(packet);
		int space_shiphp = ingameroom[p->room_id]._spaceship.getHp();
		space_shiphp -= 1;
		ingameroom[p->room_id]._spaceship.setHp(space_shiphp);

		for (auto& pl : ingameroom[p->room_id].ingamePlayer)
		{
			// 우주선 체력
			pl->send_spaceship_hp(space_shiphp);
		}
		break;
	}

	case CS_PLAYER_HIT: {
		CS_PLAYER_HIT_PACKET* p = reinterpret_cast<CS_PLAYER_HIT_PACKET*>(packet);
		clients[p->id]._isDamaged = p->isdamaged;
		clients[p->id]._hp = p->hp;

		for (auto& pl : ingameroom[p->room_id].ingamePlayer)
		{
			if (pl->_stage != clients[p->id]._stage)continue;
			if (pl->_id == p->id)continue;
			pl->send_player_hit(p->id);
		}
		break;
	}

	case CS_BOSSMONSTER_DAMGED: {

		CS_BOSSMONSTER_DAMAGED_PACKET* p = reinterpret_cast<CS_BOSSMONSTER_DAMAGED_PACKET*>(packet);
		switch (p->_montype)
		{
		case MonsterType::Fire_Boss: {

			ingameroom[p->room_id].FireBoss._hp = p->bosshp;
			cout << " Fire Boss _HP" << p->bosshp << endl;
			// 보스 죽는 애니메이션을 모두에게 보내야한다. 
			// 아이템 을 뛰우라는 정보 
			// 
			if (ingameroom[p->room_id].FireBoss._hp <= 0)
			{

				for (auto& pl : ingameroom[p->room_id].ingamePlayer)
				{
					if (pl->_stage != clients[id]._stage)continue;
					// 무슨 몬스터가 죽었는지 다른 클라이언트 들한테 정보를 보내야함 
					pl->send_player_attack_mosnter(0, true, MonsterType::Fire_Boss);

					std::chrono::system_clock::time_point attacktime = chrono::system_clock::now();
					TIMER_EVENT ev{ attacktime + 1s ,p->room_id,EVENT_TYPE::EV_PLAYER_ATTACK_NPC, 0, MonsterType::Fire_Boss };
					g_Timer.InitTimerQueue(ev);
				}
			}
		}
								   break;
		case MonsterType::Ice_Boss: {
			ingameroom[p->room_id].IceBoss._hp = p->bosshp;
			if (ingameroom[p->room_id].IceBoss._hp <= 0)ingameroom[p->room_id].FireBoss._is_alive = false;
		}
								  break;
		case MonsterType::Nature_Boss: {
			ingameroom[p->room_id].NatureBoss._hp = p->bosshp;
			if (ingameroom[p->room_id].NatureBoss._hp <= 0)ingameroom[p->room_id].FireBoss._is_alive = false;
		}
									 break;
		}
		break;
	}

	case CS_DEAD_PLAYER: {
		CS_PLAYER_DEAD_PACKET* p = reinterpret_cast<CS_PLAYER_DEAD_PACKET*>(packet);
		clients[p->id]._hp = 0;
		clients[p->id].isDead = true;

		break;


	}
	}
}

void Server::disconnect(int id)
{
	for (auto& pl : ingameroom[clients[id].room_id].ingamePlayer)
	{
		if (pl->_id == id) continue;
		pl->send_remove_packet(id);
	}

	Session* _session = &clients[id];

	ingameroom[clients[id].room_id].ingamePlayer.erase(std::remove_if(ingameroom[clients[id].room_id].ingamePlayer.begin(),
		ingameroom[clients[id].room_id].ingamePlayer.end(), [id](Session* _session) {
			return _session->_id == clients[id]._id; }), ingameroom[clients[id].room_id].ingamePlayer.end());

	//auto it = std::find_if(ingameroom[clients[id].room_id].NightMonster)

	//for (auto& npc : ingameroom[clients[id].room_id].NightMonster)
	//{
	//	auto it = find_if(npc.ingamePlayer.begin(),npc.ingamePlayer.end(), [id](Session* _session) {
	//		return _session->_id == id; });
	//	if(it != npc.ingamePlayer.end())
	//}

	ingameroom[clients[id].room_id]._state = roomState::Free;

	closesocket(clients[id]._socket);

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
		lock_guard<mutex> rl{ r_l };

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
				lock_guard<mutex> rl{ ingameroom[room_id].r_l };
				ingameroom[room_id]._state = roomState::Ingame; // 상태를 Ingame상태로 바꿔준다 
				ingameroom[room_id].ingamePlayer.emplace_back(_session);
				clients[_session->_id].room_id = room_id;

				if (ingameroom[room_id].ingamePlayer.size() != MAX_ROOM_USER)continue;
				else ingameroom[room_id].fullcheck = true;
			}

			// 접속하는 클라이언트 순서대로 array에 집어넣어야함, 
			// id를 맞춰야하나? 
			if (ingameroom[room_id].ingamePlayer.size() < MAX_ROOM_USER)continue;


			// 무조건 방에 2명이 진행 됐을 때에만 add, 서로에게 보내준다. 


			for (auto& NightMonsters : ingameroom[room_id].NightMonster)
			{
				for (int i = 0; i < MAX_ROOM_USER; ++i)
				{
					NightMonsters.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
				}
			}

			for (auto& IceMontsers : ingameroom[room_id].IceMonster)
			{
				for (int i = 0; i < MAX_ROOM_USER; ++i)
				{
					IceMontsers.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
				}
			}

			for (auto& FireMontsers : ingameroom[room_id].FireMonster)
			{
				for (int i = 0; i < MAX_ROOM_USER; ++i)
				{
					FireMontsers.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
				}
			}

			for (auto& NatureMontsers : ingameroom[room_id].NatureMonster)
			{
				for (int i = 0; i < MAX_ROOM_USER; ++i)
				{
					NatureMontsers.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
				}
			}
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				ingameroom[room_id].IceBoss.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
				ingameroom[room_id].NatureBoss.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
				ingameroom[room_id].FireBoss.ingamePlayer[i] = ingameroom[room_id].ingamePlayer[i];
			}

			for (auto& ingameplayer : ingameroom[room_id].ingamePlayer)
			{
				for (auto& player : ingameroom[room_id].ingamePlayer)
				{
					if (ingameplayer->_id == player->_id)continue;
					player->send_add_info_packet(ingameplayer->_id);
				}
			}

			if (ingameroom[room_id].ingamePlayer.size() < MAX_ROOM_USER)continue;
			cout << " Full " << room_id << " - Room " << endl;
		}
		else
			this_thread::sleep_for(500ms);

	}
}

SOCKET Server::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool Server::SetLinger(SOCKET socket, UINT16 onoff, UINT16 linger)
{
	LINGER option;
	option.l_linger = linger;
	option.l_onoff = onoff;

	return setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&option, sizeof(option));
}

bool Server::SetReuseAddress(SOCKET socket, bool flag)
{
	return false;
}

bool Server::SetTcpNoDelay(SOCKET _socket)
{
	// 서버에서 노딜레이를 해주는게 맞을까 속도보단 안전성
	int DelayZeroOpt = 1;
	return setsockopt(_socket, SOL_SOCKET, TCP_NODELAY, (const char*)&DelayZeroOpt, sizeof(DelayZeroOpt));
}


