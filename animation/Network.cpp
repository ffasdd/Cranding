#include"stdafx.h"
#include<iostream>
#include "Network.h"
#include"GameFramework.h"

extern CGameFramework					gGameFramework;

Network::Network()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		std::cout << "WSAStart Up Error " << endl;

	clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int DelayZeroOpt = 1;
	setsockopt(clientsocket, SOL_SOCKET, TCP_NODELAY, (const char*)&DelayZeroOpt, sizeof(DelayZeroOpt)); // Nodelay  네이클 알고리즘 종료 

	LINGER _linger;
	_linger.l_linger = 0;
	_linger.l_onoff = 1;
	setsockopt(clientsocket, SOL_SOCKET, SO_LINGER, (const char*)& _linger, sizeof(_linger));
	// Linger option

	if (clientsocket == INVALID_SOCKET)
	{
		std::cout << " client socket Error " << std::endl;
	}

}

Network::~Network()
{
	End();
}

bool Network::ReadytoConnect()
{
	sockaddr_in sockaddrIn;
	sockaddrIn.sin_family = AF_INET;
	sockaddrIn.sin_port = htons(PORT_NUM);

	// 사용자로부터 IP 주소 입력 받기
	string ipAddress = { "127.0.0.1"};


	// 문자열 형태의 IP 주소를 네트워크 바이트 순서로 변환하여 설정
	inet_pton(AF_INET, ipAddress.c_str(), &sockaddrIn.sin_addr);

	int ret = connect(clientsocket, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn));
	if (ret)
	{
		DWORD err = GetLastError();
		std::cout << "Connect Error" << std::endl;
		std::cout << err << endl;
		return false;
	}

	std::cout << " Success Connect " << std::endl;
	return true;
}
// 연결 완료 

// 여기까진 메인 쓰레드 
// 리시브 하는 쓰레들 새로, 
void Network::End()
{
}

void Network::StartServer()
{
	ServerStart = true;

	netThread = std::thread([this]() {NetThreadFunc(); });

	sendThread = std::thread([this]() {SendThreadFunc(); });

}

void Network::NetThreadFunc()
{
	while (ServerStart)
	{
		int ioByte = recv(clientsocket, _buf, BUF_SIZE, 0);
		ProcessData(ioByte);
	}
}
void Network::SendThreadFunc()
{
	while (ServerStart)
	{
		if (!g_sendqueue.empty())
		{
			SENDTYPE _sendtype;
			bool getcheck = g_sendqueue.try_pop(_sendtype);
			if (getcheck)
				SendProcess(_sendtype);
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(3ms));
	}
}

void Network::SendProcess(SENDTYPE sendtype)
{
	switch (sendtype)
	{
	case SENDTYPE::MOVE: {
		SendMovePlayer(g_clients[my_id].getPos());
		break;
	}
	case SENDTYPE::ROTATE: {
		SendRotatePlayer(g_clients[my_id].m_yaw);
		break;
	}
	case SENDTYPE::CHANGE_ANIMATION: {
		SendChangeAnimation(g_clients[my_id].getAnimation(), g_clients[my_id].getprevAnimation());
		break;
	}
	case SENDTYPE::CHANGE_SCENE_INGAME_START: {
		SendIngameStart();
		break;
	}
	case SENDTYPE::ATTACK: {
		SendAttack(g_clients[my_id].getAttack());
		break;
	}
	case SENDTYPE::CHANGE_STAGE: {
	
		index = static_cast<int>(gGameFramework.sceneManager.GetCurrentScene());

		SendChangeScene(index);
		break;
	case SENDTYPE::ATTACK_COLLISION:
	{
		//SendAttackCollision()
		break;
	}
	case SENDTYPE::NPC_DIE:
	{

	}
	}

	}
}

void Network::ProcessData(size_t _size)
{
	char* ptr = _buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static  char packet_buffer[BUF_SIZE];

	ZeroMemory(packet_buffer, BUF_SIZE);

	while (0 != _size) {
		if (0 == in_packet_size) in_packet_size = (unsigned char)ptr[0];
		if (_size + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			_size -= in_packet_size - saved_packet_size;

			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, _size);
			saved_packet_size += _size;
			_size = 0;
		}
	}
}

void Network::ProcessPacket(char* buf)
{
	switch (buf[1])
	{
	case SC_LOGIN_INFO: {
		// 로그인 되자마자 로그인 씬 
		SC_LOGIN_INFO_PACKET* p = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(buf);
		//my_id = getmyid(p->id);
		my_id = (p->id);
		my_roomid = p->room_id;
		g_clients[my_id].setState(STATE::Ingame);
		g_clients[my_id].setId(my_id);
		g_clients[my_id].setHp(p->hp);
		g_clients[my_id].setPos(p->pos);
		g_clients[my_id].setLook(p->look);
		g_clients[my_id].setUp(p->up);
		g_clients[my_id].setRight(p->right);
		g_clients[my_id].setCharacterType(p->charactertype);
		g_clients[my_id].setAnimation(int(p->a_state));
		g_clients[my_id].setprevAnimation(int(p->prev_state));
		gamestart = true;

		SetEvent(loginevent);

		break;
	}

	case SC_ADD_OBJECT: {

		this_thread::sleep_for(10ms);

		SC_ADD_OBJECT_PACKET* p = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(buf);
		//int ob_id = getmyid(p->id);
		int ob_id = (p->id);
		std::cout << "Add Player ID - " << ob_id << std::endl;
		g_clients[ob_id].setState(STATE::Ingame);
		g_clients[ob_id].setId(ob_id);
		g_clients[ob_id].setHp(p->hp);
		g_clients[ob_id].setPos(p->pos);
		g_clients[ob_id].setLook(p->look);
		g_clients[ob_id].setUp(p->up);
		g_clients[ob_id].setRight(p->right);
		g_clients[ob_id].setCharacterType(p->charactertype);
		g_clients[ob_id].setAnimation(int(p->a_state));
		g_clients[ob_id].setprevAnimation(int(p->prev_state));

		break;
	}

	case SC_MOVE_OBJECT: {

		SC_MOVE_OBJECT_PACKET* p = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(buf);
		//int ob_id = getmyid(p->id);
		int ob_id = (p->id);
		g_clients[ob_id].setPos(p->pos);
	}
					   break;
	case SC_ROTATE_OBJECT: {
		SC_ROTATE_OBJECT_PACKET* p = reinterpret_cast<SC_ROTATE_OBJECT_PACKET*>(buf);
		int ob_id = (p->id);
		//int ob_id = getmyid(p->id);
		float _yaw = p->yaw;
		g_clients[ob_id].Rotate(_yaw);
	}
						 break;
	case SC_CHANGE_ANIMATION: {
		SC_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<SC_CHANGE_ANIMATION_PACKET*>(buf);
		int ob_id = (p->id);
		//int ob_id = getmyid(p->id);
		g_clients[ob_id].setAnimation((int)p->a_state);
		g_clients[ob_id].setprevAnimation((int)p->prev_a_state);
	}
							break;
	case SC_START_GAME: {
		SC_GAMESTART_PACKET* p = reinterpret_cast<SC_GAMESTART_PACKET*>(buf);
		my_roomid = p->roomid;
		//Start가 되었을 때 인게임 씬으로 이동 

		break;
	}
	case SC_ATTACK: {
		SC_ATTACK_PACKET* p = reinterpret_cast<SC_ATTACK_PACKET*>(buf);
		int ob_id = p->id;
		g_clients[ob_id].setAttack(p->isAttack);

		break;
	}

	case SC_CHANGE_SCENE: {
		SC_CHANGE_SCENE_PACKET* p = reinterpret_cast<SC_CHANGE_SCENE_PACKET*>(buf);
		int ob_id = p->id;

		g_clients[ob_id].scene_num = p->stage;
		g_clients[ob_id].setPos(p->pos);
		if (ob_id == my_id) // 내가 씬전환을 했다면
		{
			stage_num = p->stage;

			switch (stage_num)
			{
			case 1:
				// 로비씬.  아무것도안해도됨 
				break;
			case 2:
				// 우주선 씬 ,
				if (IngameScene == false)
				{
					IngameScene = true;
					ClientState = true;

				}
				else if ( IngameScene == true )
				{
					SpaceshipScene = true;
					
					for (int i = 0; i < g_clients.size(); ++i)
					{
						if (g_clients[i].getId() == my_id)continue;
						if (g_clients[i].scene_num != g_clients[my_id].scene_num)
						{
						
						//	gGameFramework.myFunc_SetBlind(i, ob_id, false);

						}
						//else
						{
				
							//gGameFramework.myFunc_SetBlind(i, ob_id, true);

						}
					}
				}
				break;

			case 3:
			{
				for (int i = 0; i < g_clients.size(); ++i)
				{
					if (g_clients[i].getId() == my_id)continue;
					//if (g_clients[i].scene_num != g_clients[my_id].scene_num)
					//	gGameFramework.myFunc_SetBlind(i, ob_id, false);
				//	else
					//gGameFramework.myFunc_SetBlind(i, ob_id, true);
				}
				g_monsters.clear();
			}
				break;
			case 4 :
			{
				for (int i = 0; i < g_clients.size(); ++i)
				{
					if (g_clients[i].getId() == my_id)continue;
					//if (g_clients[i].scene_num != g_clients[my_id].scene_num)
						//gGameFramework.myFunc_SetBlind(i, ob_id, false);
					//else
						//gGameFramework.myFunc_SetBlind(i, ob_id, true);
				}
				g_monsters.clear();
			}
				break;
			case 5:
			{
				for (int i = 0; i < g_clients.size(); ++i)
				{
					if (g_clients[i].getId() == my_id)continue;
					//if (g_clients[i].scene_num != g_clients[my_id].scene_num)
						//gGameFramework.myFunc_SetBlind(i, ob_id, false);
					//else
					//	gGameFramework.myFunc_SetBlind(i, ob_id, true);
				}
				g_monsters.clear();
			}
				break;
			}
		}
		else
		{
			for (int i = 0; i < g_clients.size(); ++i)
			{
				if (p->stage == 1)continue;
				if (g_clients[i].getId() == my_id)continue;
				//if (g_clients[i].scene_num != g_clients[my_id].scene_num)
				//	gGameFramework.myFunc_SetBlind(i, ob_id, false);
				//else
				//	gGameFramework.myFunc_SetBlind(i, ob_id, true);
			}
		}
		// Id가 하나만 ㅁ거음 

		break;
	}
	case SC_INGAME_STRAT: {
		IngameStart = true;
		break;
	}
	case SC_REMOVE_OBJECT: {
		SC_REMOVE_OBJECT_PACKET* p = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(buf);
		int ob_id = p->id;
		g_clients[ob_id].setState(STATE::Free);
		g_clients.erase(ob_id);
		break;
	}

	case SC_MONSTER_UPDATE_POS: {

		if (stage_num == 2)
		{
			// 10 개로 받아줘야한다. 
			NightMonstersUpdate* p = reinterpret_cast<NightMonstersUpdate*>(buf);
			int npc_id = p->_monster._id;
			g_monsters[npc_id].setId(npc_id);
			g_monsters[npc_id].setPrevPos(g_monsters[npc_id].getPos());
		
			g_monsters[npc_id].setPos(p->_monster._x, p->_monster._y, p->_monster._z);
			g_monsters[npc_id].setLook(p->_monster._lx, p->_monster._ly, p->_monster._lz);
			g_monsters[npc_id].setRight(p->_monster._rx, p->_monster._ry, p->_monster._rz);
			g_monsters[npc_id].setUp({ 0.f,1.f,0.f });
			g_monsters[npc_id].scene_num = 2;
		}
	
		break;
	}
	case SC_DAYTIME:
	{
		SC_DAYTIME_PACKET* p = reinterpret_cast<SC_DAYTIME_PACKET*>(buf);
		gGameFramework.DayTime = true;
		gGameFramework.Night = false;
		cout << " Day Time " << endl;
		break;
	}
	case SC_NIGHT:
	{
		SC_NIGHT_PACKET* p = reinterpret_cast<SC_NIGHT_PACKET*>(buf);
		gGameFramework.DayTime = false;
		gGameFramework.Night = true;
		cout << " Night " << endl;
		break;
	}
	case SC_ICE_MONSTER_UPDATE :
	{
		if (stage_num != 3) break;
		IceMonstersUpdate* p = reinterpret_cast<IceMonstersUpdate*>(buf);
		int npc_id = p->_monster._id;

		g_ice_monsters[npc_id].setId(npc_id);
		g_ice_monsters[npc_id].setPos(p->_monster._x, p->_monster._y, p->_monster._z);
		g_ice_monsters[npc_id].setLook(p->_monster._lx, p->_monster._ly, p->_monster._lz);
		g_ice_monsters[npc_id].setRight(p->_monster._rx, p->_monster._ry, p->_monster._rz);

		g_ice_monsters[npc_id].setUp({ 0.f,1.f,0.f });
	}
	break;

	case SC_FIRE_MONSTER_UPDATE:
	{
		if (stage_num != 4)break;
		FireMonsterUpdate* p = reinterpret_cast<FireMonsterUpdate*>(buf);
		int npc_id = p->_monster._id;

		g_fire_monsters[npc_id].setId(npc_id);
		g_fire_monsters[npc_id].setPos(p->_monster._x, p->_monster._y, p->_monster._z);
		g_fire_monsters[npc_id].setLook(p->_monster._lx, p->_monster._ly, p->_monster._lz);
		g_fire_monsters[npc_id].setRight(p->_monster._rx, p->_monster._ry, p->_monster._rz);

		g_fire_monsters[npc_id].setUp({ 0.f,1.f,0.f });
	}
	break;

	case SC_NATURE_MONSTER_UPDATE:
	{
		if (stage_num != 5)break;
		NatureMonsterUpdate* p = reinterpret_cast<NatureMonsterUpdate*>(buf);
		int npc_id = p->_monster._id;

		g_nature_monsters[npc_id].setId(npc_id);
		g_nature_monsters[npc_id].setPos(p->_monster._x, p->_monster._y, p->_monster._z);
		g_nature_monsters[npc_id].setLook(p->_monster._lx, p->_monster._ly, p->_monster._lz);
		g_nature_monsters[npc_id].setRight(p->_monster._rx, p->_monster._ry, p->_monster._rz);

		g_nature_monsters[npc_id].setUp({ 0.f,1.f,0.f });
	}
	break;
	case SC_ICE_BOSS_UPDATE:
	{
		if (stage_num != 3)break;
		BossUpdate_Ice* p = reinterpret_cast<BossUpdate_Ice*>(buf);
		g_IceBossMonster.setPos(p->_boss._x, p->_boss._y, p->_boss._z);
		g_IceBossMonster.setLook(p->_boss._lx, p->_boss._ly, p->_boss._lz);
		g_IceBossMonster.setRight(p->_boss._rx, p->_boss._ry, p->_boss._rz);

		g_IceBossMonster.setUp({ 0.f,1.f,0.f });
	}
	break;

	case SC_FIRE_BOSS_UPDATE:
	{
		if (stage_num != 4)break;
		BossUpdate_Fire* p = reinterpret_cast<BossUpdate_Fire*>(buf);
		g_FireBossMonster.setPos(p->_boss._x, p->_boss._y, p->_boss._z);
		g_FireBossMonster.setLook(p->_boss._lx, p->_boss._ly, p->_boss._lz);
		g_FireBossMonster.setRight(p->_boss._rx, p->_boss._ry, p->_boss._rz);

		g_FireBossMonster.setUp({ 0.f,1.f,0.f });
	}
	break;

	case SC_NATURE_BOSS_UPDATE:
	{
		if (stage_num != 5)break;
		BossUpdate_Nature* p = reinterpret_cast<BossUpdate_Nature*>(buf);
		g_NatureBossMonster.setPos(p->_boss._x, p->_boss._y, p->_boss._z);
		g_NatureBossMonster.setLook(p->_boss._lx, p->_boss._ly, p->_boss._lz);
		g_NatureBossMonster.setRight(p->_boss._rx, p->_boss._ry, p->_boss._rz);

		g_NatureBossMonster.setUp({ 0.f,1.f,0.f });
	}
	break;
	case SC_MONSTER_ATTACK:
	{	
		SC_MONSTER_ATTACK_PACKET* p = reinterpret_cast<SC_MONSTER_ATTACK_PACKET*>(buf);
		
		switch (p->monstertype)
		{
		case MonsterType::Fire: {
			if (p->is_attack)
			{
				cout << "(Fire Attack) " << endl;
				g_fire_monsters[p->id].setNpcAttack(p->is_attack);
			}
			break;
		}
		case MonsterType::Ice: {
			if (p->is_attack)
			{
				cout << "(Ice Attack) " << endl;
				g_ice_monsters[p->id].setNpcAttack(p->is_attack);
			}
			break;
		}
		case MonsterType::Nature: {
			if (p->is_attack)
			{
				cout << "(Nature Attack) " << endl;
				g_nature_monsters[p->id].setNpcAttack(p->is_attack);
			}
			break;
		}
		case MonsterType::Ice_Boss: {
			if (p->is_attack)
			{
				g_IceBossMonster.setNpcAttack(p->is_attack);
				cout << " Player Attack " << endl;
			}
			break;
		}
		}

	}
	break;
	case  SC_MONSTER_DIE:
	{
		SC_MONSTER_DIE_PACKET* p = reinterpret_cast<SC_MONSTER_DIE_PACKET*>(buf);
		cout << " SC_MONSTER_DIE" << endl;

		int npc_id = p->npc_id;
		g_monsters[npc_id].setNpcAttacked(true);
	}
	break;
	}
}

void Network::SendLoginfo()
{
	CS_LOGIN_PACKET p;
	strcpy_s(p.name, "Test");
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;
	p.charactertype = 0;

	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendTest()
{
	CS_TEST_PACKET p;
	p.size = sizeof(CS_TEST_PACKET);
	p.type = CS_TEST;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendMovePlayer(XMFLOAT3 _pos)
{
	CS_MOVE_PACKET p;
	p.size = sizeof(CS_MOVE_PACKET);
	p.type = CS_MOVE;
	p.pos = _pos;
	p.roomid = my_roomid;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendRotatePlayer(float _yaw)
{
	CS_ROTATE_PACKET p;
	p.size = sizeof(CS_ROTATE_PACKET);
	p.type = CS_ROTATE;
	p.yaw = _yaw;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendChangeAnimation(int curanimate, int prevanimate)
{
	CS_CHANGE_ANIMATION_PACKET p;
	p.size = sizeof(CS_CHANGE_ANIMATION_PACKET);
	p.type = CS_CHANGE_ANIMATION;
	p.a_state = curanimate;
	p.prev_a_state = prevanimate;
	p.roomid = my_roomid;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendChangeScene(int scenenum)
{
	CS_CHANGE_SCENE_PACKET p;
	p.size = sizeof(CS_CHANGE_SCENE_PACKET);
	p.type = CS_CHANGE_SCENE;
	p.roomid = my_roomid;
	p.scenenum = scenenum;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendIngameStart()
{
	CS_INGAME_START_PACKET p;
	p.size = sizeof(CS_INGAME_START_PACKET);
	p.type = CS_INGAME_START;
	p.roomid = my_roomid;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendAttack(bool is_attack)
{
	CS_ATTACK_PACKET p;
	p.size = sizeof(CS_ATTACK_PACKET);
	p.type = CS_ATTACK;
	p.isAttack = is_attack;
	p.roomid = my_roomid;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendAttackCollision(int npc_id,MonsterType _mtype)
{
	CS_ATTACK_COLLISION_PACKET p;
	p.size = sizeof(CS_ATTACK_COLLISION_PACKET);
	p.type = CS_ATTACK_COLLISION;
	p.npc_id = npc_id;
	p.room_id = my_roomid;
	p._montype = _mtype;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendReady()
{
	CS_READY_PACKET p;
	p.size = sizeof(CS_READY_PACKET);
	p.type = CS_READY_GAME;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendTime(int time)
{
	CS_TIME_CHECK_PACKET p;
	p.size = sizeof(CS_TIME_CHECK_PACKET);
	p.type = CS_TIME_CHECK;
	p.roomid = my_roomid;
	p.time = time;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendMonsterDie(int npc_id, MonsterType _mtype)
{
	CS_MONSTER_DIE_PACKET p;
	p.type = CS_MONSTER_DIE;
	p.size = sizeof(CS_MONSTER_DIE_PACKET);
	p._montype = _mtype;
	p.room_id = my_roomid;
	p.npc_id = npc_id;

	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

bool Network::MonsterCollide(Session& _monster)
{
	return 0;
}


int Network::getmyid(int _id)
{
	if (_id > 2)
	{
		if (_id % 3 == 0)
		{
			return 0;
		}
		else if (_id % 2 == 0)
			return 2;
		else
			return 1;

	}
	else
		return _id;
}
