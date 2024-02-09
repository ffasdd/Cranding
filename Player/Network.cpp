#include "stdafx.h"
#include "Network.h"

array<Session, 3> clients;

Network& Network::GetInstance()
{
	static Network instance;
	return instance;
	// TODO: ���⿡ return ���� �����մϴ�.

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

void Network::Login()
{
	CS_LOGIN_PACKET p;
	int _id = 0;
	cout << " ID : " << endl;
	cin >> _id;
	cout << " NAME : " << endl;
	char name[NAME_SIZE];
	cin >> name;
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;
	p.id = _id;
	strcpy_s(p.name, name);

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
			// �̹� ����� ���¶�� break
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
	do_recv();
	// ���⼭ ���ӷ����� ������ key input 
	// Ű�� �޴� ť�� �ʿ�? 


}


void Network::Login_send()
{
	CS_LOGIN_PACKET p;
	int _id = 1;

	char name[NAME_SIZE] = { "SDY" };

	p.id = _id;
	strcpy_s(p.name, name);
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = CS_LOGIN;

	OVER_EX* send_data = new OVER_EX{ reinterpret_cast<char*>(&p) };

	if (WSASend(clientSocket, &send_data->wsabuf, 1, 0, 0, &send_data->overlapped, 0) == SOCKET_ERROR)
		cout << " Send Error" << endl;
}

void Network::do_recv()
{
	DWORD recv_flag = 0;
	memset(&recv_over.overlapped, 0, sizeof(recv_over.overlapped));
	recv_over.wsabuf.len = BUF_SIZE;
	recv_over.wsabuf.buf = recv_over.send_buf;

	if (WSARecv(clientSocket, &recv_over.wsabuf, 1, 0, &recv_flag, &recv_over.overlapped, 0) == SOCKET_ERROR)
	{
		if (GetLastError() != WSA_IO_PENDING)
			return;
	}


	int recv_num = recv_over.wsabuf.len;
	char* ptr = recv_over.wsabuf.buf;
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
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(&buf);
		my_id = packet->id;
	
		//clients[my_id].m_id = packet->id;
		break;

	}
	case  SC_ADD_OBJECT:
	{
		int ob_id;
		std::cout << "Add Player" << std::endl;
		SC_ADD_OBJECT_PACKET* packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(&buf);
		ob_id = packet->id;
		clients[ob_id].m_id = packet->id;
		break;
	}
	case SC_MOVE_OBJECT:
	{
		break;
	}

	}
}

