#include"stdafx.h"
#include<iostream>
#include "Network.h"

Network::Network()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		std::cout << "WSAStart Up Error " << endl;

	clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
	sockaddrIn.sin_port = htons(9000);
	inet_pton(AF_INET, "127.0.0.1", &sockaddrIn.sin_addr);

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
	netThread = std::thread([this]() {NetThreadFunc(); });
	sendThread = std::thread([this]() {SendThreadFunc(); });

	ServerStart = true;
}

void Network::NetThreadFunc()
{
	while (ServerStart)
	{
		int ioByte = recv(clientsocket, _buf, BUF_SIZE, 0);

		ProcessData(ioByte);

		cout << " recvByte : " << ioByte << endl;

	}
}
void Network::SendThreadFunc()
{
	while (ServerStart)
	{
		if (!g_sendqueue.empty())
		{
			SendProcess(g_sendqueue.front());
			g_sendqueue.pop();

		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1ms));
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
		SendRotatePlayer(g_clients[my_id].getLook(),g_clients[my_id].getRight(),g_clients[my_id].getUp());
	}
	case SENDTYPE::CHANGE_ANIMATION: {
		SendChangeAnimation((animateState)g_clients[my_id].getAnimation(),(animateState)g_clients[my_id].getprevAnimation());
	}
	}
}

void Network::ProcessData(size_t _size)
{
	char* ptr = _buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != _size) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
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

		SC_LOGIN_INFO_PACKET* p = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(buf);
		my_id = p->id;

		g_clients[my_id].setId(p->id);
		g_clients[my_id].setHp(p->hp);
		g_clients[my_id].setPos(p->pos);
		g_clients[my_id].setLook(p->look);
		g_clients[my_id].setUp(p->up);
		g_clients[my_id].setRight(p->right);
		g_clients[my_id].setCharacterType(p->charactertype);
		cout << " Recv Login Info " << endl;


		break;
	}

	case SC_ADD_OBJECT: {

		std::cout << "Add Player " << std::endl;
		SC_ADD_OBJECT_PACKET* p = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(buf);
		int ob_id = p->id;
		g_clients[ob_id].setId(ob_id);
		g_clients[ob_id].setHp(p->hp);
		g_clients[ob_id].setPos(p->pos);
		g_clients[ob_id].setLook(p->look);
		g_clients[ob_id].setUp(p->up);
		g_clients[ob_id].setRight(p->right);
		g_clients[ob_id].setCharacterType(p->charactertype);
		cout << " Match ID - " << ob_id << endl;
		break;
	}
					  break;
	case SC_MOVE_OBJECT: {
		SC_MOVE_OBJECT_PACKET* p = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(buf);
		int ob_id = p->id;
		std::cout << ob_id << " Player Move " << endl;
		g_clients[ob_id].setPos(p->pos);
	}
					   break;
	case SC_ROTATE_OBJECT: {
		SC_ROTATE_OBJECT_PACKET* p = reinterpret_cast<SC_ROTATE_OBJECT_PACKET*>(buf);
		int ob_id = p->id;
		g_clients[ob_id].setLook(p->look);
		g_clients[ob_id].setRight(p->right);
		g_clients[ob_id].setUp(p->up);
	}
						 break;
	case SC_CHANGE_ANIMATION: {
		SC_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<SC_CHANGE_ANIMATION_PACKET*>(buf);
		int ob_id = p->id;
		g_clients[ob_id].setAnimation(p->a_state);
		g_clients[ob_id].setprevAnimation(p->prev_a_state);
	}
							break;


	case SC_TEST: {
		SC_TEST_PACKET* p = reinterpret_cast<SC_TEST_PACKET*>(buf);
		cout << " Test Success " << endl;
		break;
	}
	}
}

void Network::SendLoginfo(char* name)
{
	CS_LOGIN_PACKET p;
	strcpy_s(p.name, name);
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
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendRotatePlayer(XMFLOAT3 _look, XMFLOAT3 _right, XMFLOAT3 _up)
{
	CS_ROTATE_PACKET p;
	p.size = sizeof(CS_ROTATE_PACKET);
	p.type = CS_ROTATE;
	p.look = _look;
	p.right = _right;
	p.up = _up;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);

}

void Network::SendChangeAnimation( animateState curanimate,animateState prevanimate )
{
	CS_CHANGE_ANIMATION_PACKET p;
	p.size = sizeof(CS_CHANGE_ANIMATION_PACKET);
	p.type = CS_CHANGE_ANIMATION;
	p.a_state = curanimate;
	p.prev_a_state = prevanimate;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}

void Network::SendReady()
{
	CS_READY_PACKET p;
	p.size = sizeof(CS_READY_PACKET);
	p.type = CS_READY_GAME;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}
