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
		SocketUtils::Init(); // 윈속 초기화 
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
		
	}

public:
} GCoreGlobal;