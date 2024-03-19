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
// ���� �Ϸ� 

// ������� ���� ������ 
// ���ú� �ϴ� ������ ����, 


void Network::End()
{
}

void Network::StartServer()
{
	netThread = std::thread([this]() {NetThreadFunc(); });
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
		int p_id = p->id;
		g_clients[p_id].setId(p_id);
		g_clients[p_id].setHp(p->hp);
		g_clients[p_id].setPos(p->pos);
		g_clients[p_id].setLook(p->look);
		g_clients[p_id].setUp(p->up);
		g_clients[p_id].setRight(p->right);

		SetEvent(g_event);
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

	}
						 break;
	}
}

void Network::SendLoginfo(char* name)
{
	CS_LOGIN_PACKET p;
	strcpy_s(p.name, name);
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;
	send(clientsocket, reinterpret_cast<char*>(&p), p.size, 0);
}
