#pragma once
class Network
{
public:
	static Network& GetInstance();

	Network();
	Network(const Network& network) = delete;
	Network& operator=(const Network& network) = delete;

	~Network();

	void Run();
	void Close();

private:
	SOCKET clientSocket;
	char sendbuf[100];
	WSABUF wsaBuf;

};

