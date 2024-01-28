#include "pch.h"
#include "Over_Exp.h"

Over_Exp::Over_Exp(COMP_TYPE type) : _type(type)
{
	Init();
}


void Over_Exp::Init()
{
	_over.hEvent = 0;
	_over.Internal = 0;
	_over.InternalHigh = 0;
	_over.Offset = 0;
	_over.OffsetHigh = 0;
}