#include"stdafx.h"
#include"network.h"

network iocpnetwork;

int main()
{
	iocpnetwork.WSAinitialize();
	iocpnetwork.Start();
}