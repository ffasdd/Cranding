#include"pch.h"

#include <iostream>
#include"CorePch.h"
#include"CoreMacro.h"
#include"ThreadManager.h"
#include"SocketUtils.h"
#include"IocpCore.h"
#include"Over_Exp.h"


int main()
{

	// 소켓 설정  글로벌 소켓 필요 g_s_socket 
	SOCKET s_socket = SocketUtils::CreateSocket();
	
	SocketUtils::BindAnyAddress(s_socket, 7777);
	
	SocketUtils::Listen(s_socket);
	// IOCP 등록 서버소켓 등록 
	GIocpCore.Register(&s_socket);
	// clinet socket 
	SOCKET c_socket = SocketUtils::CreateSocket();

	Over_Exp a_over(COMP_TYPE::Accept);

	// AcceptEx  하면서 컨테이너에 저장


}


 