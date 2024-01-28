#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"


ThreadManager* GThreadManager = nullptr;


class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		SocketUtils::Init(); // ���� �ʱ�ȭ 
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
		
	}

public:
} GCoreGlobal;