#pragma once
#include <iostream>

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <thread>
#include <array>
#include <vector>
#include <unordered_set>
#include <concurrent_priority_queue.h>

#include <mutex>
#include<string>


#include <sqlext.h> 

// DirectX12 

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;


#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

enum S_STATE
{
	ST_FREE,
	ST_ALLOC, 
	ST_INGAME
};

enum COMP_TYPE
{
	OP_ACCEPT,
	OP_RECV, 
	OP_SEND,
	OP_NPC_MOVE,
	OP_AI_TOUCH
};

enum EVENT_TYPE
{
	EV_RANDOM_MOVE,
	EV_SAVE_POS
};

constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 256;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 20;
constexpr int MAX_USER = 100;

