#include "pch.h"
#include "Over_Exp.h"

Over_Exp::Over_Exp()
{
	_wsaBuf.buf = _sendbuf;
	_wsaBuf.len = 200;
	_comptype = COMP_TYPE::Recv;
	ZeroMemory(&_over, sizeof(_over));
}

Over_Exp::Over_Exp(char* packet)
{
	_wsaBuf.len = (unsigned char)packet[0];
	_wsaBuf.buf = _sendbuf;
	ZeroMemory(&_over, sizeof(_over));
	_comptype = COMP_TYPE::Send;
	memcpy(_sendbuf, packet, _wsaBuf.len);
}
