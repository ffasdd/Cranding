#include"stdafx.h"
#include"protocol.h"
#include"Over_Exp.h"
#include"Iocp.h"

int main()
{
	// 소켓 생성 
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	GIocp.Register(c_socket);
	g_a_over._type = COMP_TYPE::Accept;
	AcceptEx(s_socket, c_socket, g_a_over._sendbuf,0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

	vector<thread> worker_thread;
	int num_threads = std::thread::hardware_concurrency();

	for (int i = 0; i < num_threads; ++i)
	{
		worker_thread.emplace_back(&Iocp::WorkerThread);
	}

	// IOCP생성
	// 생성하면서 오버랩 리슨소켓, 클라소켓 다 setting 

	// IOCP 등록

	// worker Thread 실행


}