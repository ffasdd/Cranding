#include "pch.h"
#include "Session.h"
#include"SocketUtils.h"
Session::Session()
{
    _clientsocket = SocketUtils::CreateSocket();
}

Session::~Session()
{
    SocketUtils::Close(_clientsocket);
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(_clientsocket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numofBytes)
{
}
