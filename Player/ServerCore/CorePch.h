#pragma once

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include <fstream>
#include<vector>
#include<list>
#include<queue>
#include<stack>
#include<map>
#include<set>
#include<unordered_map>
#include<unordered_set>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <Mmsystem.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

extern HINSTANCE						ghAppInstance;

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

#define FRAME_BUFFER_WIDTH				640
#define FRAME_BUFFER_HEIGHT				480

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

#include<iostream>
using namespace std;

#include <winsock2.h>
#include<MSWSock.h>
#include<ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HelloWorld();
