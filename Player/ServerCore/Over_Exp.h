#pragma once

enum class COMP_TYPE : uint8
{
	Connect,
	Accept,
	Recv,
	Send
};

class Over_Exp 
{
public:
	WSAOVERLAPPED _over;
	WSABUF _wsaBuf;
	char _sendbuf[200];

	Over_Exp() {};
	~Over_Exp() {};
	Over_Exp(COMP_TYPE type);
	
	void Init();
	COMP_TYPE GetType() { return _type; }
private:
	COMP_TYPE _type;
};
