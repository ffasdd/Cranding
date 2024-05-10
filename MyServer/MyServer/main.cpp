#include"Iocp.h"

const UINT16 SERVERPORT = 7000;
const UINT16 MAX_CLIENT = 10000;


int main()
{
	Iocp server;
	server.InitSocketSet();
	server.StartServer();
}