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
}

void Network::Close()
{
}
