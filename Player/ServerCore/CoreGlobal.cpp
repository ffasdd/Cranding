#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "Over_exp.h"

ThreadManager* GThreadManager = nullptr;
Over_Exp* a_over = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		SocketUtils::Init(); // 윈속 초기화 
		a_over = new Over_Exp(COMP_TYPE::Accept);
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		SocketUtils::Clear();
		delete a_over;

	}

public:
} GCoreGlobal;