#pragma once
class Network : OVERLAPPED
{
public:


private:
	SOCKET clientSocket;
	char sendbuf[100];
	WSABUF wsaBuf;

};

