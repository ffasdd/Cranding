#include"pch.h"
#include <iostream>
#include"CorePch.h"
#include"CoreMacro.h"
#include"ThreadManager.h"
#include"SocketUtils.h"
#include"IocpCore.h"
#include"Over_Exp.h"	 

// 컨테이너 생성 


int main()
{
	SOCKET s_socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddress(s_socket, 7777);

	SocketUtils::Listen(s_socket);
	// IOCP 등록 서버소켓 등록 
	if (false == GIocpCore.Register(s_socket))
	{
		cout << " false " << endl;
	}

	// client socket 
	SOCKET c_socket = SocketUtils::CreateSocket();

	Over_Exp* a_over = new Over_Exp(COMP_TYPE::Accept);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	if (false == SocketUtils::acceptEx(s_socket, c_socket, a_over->_sendbuf, 0, addr_size + 16, addr_size + 16, 0, &a_over->_over))
	{
		const int32 errorcode = ::WSAGetLastError();
		if (errorcode != WSA_IO_PENDING)
			cout << "accept err" << errorcode << endl;
	}

	int num_threads = thread::hardware_concurrency();
	//auto workerThread = [&GIocpCore]() {GIocpCore.Dispatch(); };
	for (int i = 0; i < num_threads; ++i)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					GIocpCore.Dispatch(c_socket);
				}
			});
	}
	GThreadManager->Join();

	WSACleanup();
}


 