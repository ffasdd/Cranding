#include "pch.h"
#include "Over_Exp.h"

Over_Exp::Over_Exp(COMP_TYPE type) : _type(type)
{
	Init();
}

void Over_Exp::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}
