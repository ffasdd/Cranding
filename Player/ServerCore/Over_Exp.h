#pragma once

enum class COMP_TYPE : uint8
{
	Connect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

class Over_Exp : public OVERLAPPED
{
public:
	char _sendbuf[200];
	WSABUF _wsaBuf;
	Over_Exp(COMP_TYPE type);

	void Init();
	COMP_TYPE GetType() { return _type; }
private:
	COMP_TYPE _type;
};
