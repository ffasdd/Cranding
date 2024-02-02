#include "pch.h"
#include "Session.h"

void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&recv_over._over, 0, sizeof(recv_over._over));
	recv_over._wsaBuf.len = BUFSIZ - _prevremain;
	recv_over._wsaBuf.buf = recv_over._sendbuf + _prevremain;
	WSARecv(_socket, &recv_over._wsaBuf, 1, 0, &recv_flag,
		&recv_over._over, 0);
}

void Session::do_send(void* packet)
{
}
