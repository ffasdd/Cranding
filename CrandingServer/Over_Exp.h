#pragma once
class Over_Exp
{
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _sendbuf[BUF_SIZE];
	COMP_TYPE _comptype;

public:
	Over_Exp()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _sendbuf;
		_comptype = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	Over_Exp(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _sendbuf;
		ZeroMemory(&_over, sizeof(_over));
		_comptype = OP_SEND;
		memcpy(_sendbuf, packet, packet[0]);
		
	}

};

