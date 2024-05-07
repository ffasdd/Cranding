#include"pch.h"
#include"Server.h"

HANDLE _IocpHandle;

int main()
{

	Server& server = Server::GetInstance();

	server.Run();
}
