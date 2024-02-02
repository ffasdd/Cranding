#pragma once
enum class  COMP_TYPE : unsigned int { Connect, Accept, Recv, Send };
class Over_Exp
{
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _sendbuf[BUF_SIZE];
	COMP_TYPE _type;

	Over_Exp();
	Over_Exp(char* packet);
};

extern Over_Exp g_a_over;
