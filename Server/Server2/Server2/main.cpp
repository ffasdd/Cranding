#include"pch.h"
#include"Server.h"


HANDLE _IocpHandle;
Timer g_Timer;
int main()
{

	Server& server = Server::GetInstance();

	server.Run();
}
