#pragma once
enum class COMP_TYPE : int { Accept , Recv , Send };
class Over_Exp
{
public:
	Over_Exp();
	Over_Exp(char* packet);
public:
	WSAOVERLAPPED _over;
	WSABUF _wsaBuf;
	char _sendbuf[200];
	COMP_TYPE _comptype;
};
