#include "stdafx.h"
#include "Network.h"

Network& Network::GetInstance()
{
	static Network instance;
	return instance;
	// TODO: 여기에 return 문을 삽입합니다.
}

Network::Network()
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Network::~Network()
{
	closesocket(clientSocket);
	WSACleanup();
}

void Network::Run()
{
	clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in clientaddr;
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(9000);
	inet_pton(AF_INET, "127.0.0.1", &clientaddr.sin_addr);
	while (true)
	{
		if (connect(clientSocket, (sockaddr*)&clientaddr, sizeof(clientaddr)) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			// 이미 연결된 상태라면 break
			if (::WSAGetLastError() == WSAEISCONN)

				break;
			__int32 errcode = ::WSAGetLastError();
			cout << " Error :: " << errcode << endl;
			break;
		}
	}
	cout << " Connect Server " << endl;

	//LOGIN 
	Login_send();

	while (true)
		recvPacket();

}


void Network::Login_send()
{
	CS_LOGIN_PACKET p;
	char name[NAME_SIZE];
	cout << "Input User Name : ";
	cin >> name;
	cout << "Input User Id (Only Number): ";
	int id;
	cin >> id;

	strcpy_s(p.name, name);
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;
	p.id = id;
	OVER_EX* send_data = new OVER_EX{ reinterpret_cast<char*>(&p) };

	if (WSASend(clientSocket, &send_data->wsabuf, 1, 0, 0, &send_data->overlapped, 0) == SOCKET_ERROR)
		cout << " Send Error" << endl;
}



void Network::recvPacket()
{
	char buf[BUF_SIZE] = { 0 };
	WSABUF wsabuf{ BUF_SIZE,buf };
	DWORD recv_byte{ 0 }, recv_flag{ 0 };

	if (WSARecv(clientSocket, &wsabuf, 1, &recv_byte, &recv_flag, 0, 0) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			return;
	}
	if (recv_byte > 0)
		processData(wsabuf.buf, recv_byte);
}

void Network::processData(CHAR* buf, size_t recv_num)
{
	char* ptr = buf;
	static size_t packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != recv_num)
	{
		if (0 == packet_size) packet_size = ptr[0];
		if (recv_num + saved_packet_size >= packet_size)
		{
			memcpy(packet_buffer + saved_packet_size, ptr, packet_size - saved_packet_size);
			processPacket(packet_buffer);
			ptr += packet_size - saved_packet_size;
			recv_num -= packet_size - saved_packet_size;
			packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{
			memcpy(packet_buffer + saved_packet_size, ptr, recv_num);
			saved_packet_size += recv_num;
			recv_num = 0;
		}
	}
}

void Network::processPacket(char* buf)
{
	switch (buf[1])
	{
	case SC_LOGIN_INFO:
	{
		std::cout << "success Login" << std::endl;
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(buf);
		my_id = packet->id;
		clients[my_id].m_id = my_id;
		clients[my_id].m_hp = packet->hp;
		clients[my_id]._pos = packet->pos;
		clients[my_id]._look_vec = packet->look;
		clients[my_id]._right_vec = packet->right;
		clients[my_id]._up_vec = packet->up;
		clients[my_id].A_STATE = packet->a_state;
		SetEvent(g_event);
		break;
	}
	case  SC_ADD_OBJECT:
	{

		std::cout << "Add Player " << std::endl;
		SC_ADD_OBJECT_PACKET* packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(buf);
		int ob_id = packet->id;

		clients[ob_id].m_id = ob_id;
		clients[ob_id].m_hp = packet->hp;
		strcpy_s(clients[ob_id].m_name, packet->name);
		clients[ob_id]._pos = packet->pos;
		clients[ob_id]._look_vec = packet->look;
		clients[ob_id]._right_vec = packet->right;
		clients[ob_id]._up_vec = packet->up;
		clients[ob_id].A_STATE = packet->a_state;
		


		break;
	}
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* p = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(buf);
		int cl_id = p->id;

		clients[cl_id]._pos = p->pos;
		clients[cl_id].A_STATE = p->a_state;

		break;
	}
	case SC_ROTATE_OBJECT:
	{
		SC_ROTATE_OBJECT_PACKET* p = reinterpret_cast<SC_ROTATE_OBJECT_PACKET*>(buf);
		int cl_id = p->id;

		clients[cl_id]._look_vec = p->look;
		clients[cl_id]._right_vec = p->right;
		clients[cl_id]._up_vec = p->up;
		break;
	}
	case SC_REMOVE_OBJECT: {
		SC_REMOVE_OBJECT_PACKET* p = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(buf);
		int ob_id = p->id;
		clients.erase(ob_id);

		break;
	}

	}
}

void Network::sendprocess(int sendtype)
{
	switch (sendtype)
	{
	case 1:
		cout << "move forward " << endl;
		break;
	}
}

