//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "UI.h"

#define monsternum 10

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	m_pPlayer = NULL;

	_tcscpy_s(m_pszFrameRate, _T("Cranding ("));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();

	CreateSwapChain();

	CreateSwapChainRenderTargetViews();

	CreateDepthStencilView();

	CoInitialize(NULL);

	BuildObjects(0);

	return(true);
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);

#endif

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);

	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break;
	}

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void**)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	HRESULT hResult;

	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers + 5;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	// **
	//::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	//::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateSwapChainRenderTargetViews()
{
	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		m_pd3dSwapChainBackBufferRTVCPUHandles[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}
}



void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

	m_d3dDsvDescriptorCPUHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, m_d3dDsvDescriptorCPUHandle);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateSwapChainRenderTargetViews();
}

// ���콺 �Է�, ����
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		// ��Ŭ������ ����
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		// 플레이어의 m_bIsDead가 true면 공격 패킷 보내면 안됨!!!!!!
		if (g_clients[cl_id].getCharacterType() == 0)
		{
			g_clients[cl_id].setAttack(true);
			gNetwork.SendAttack(g_clients[cl_id].getAttack());
		}
		break;


	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		m_pPlayer->m_pSkinnedAnimationController->m_bisRotate = true;
		break;

	case WM_LBUTTONUP:
		::ReleaseCapture();
		break;

	case WM_RBUTTONUP:
		::ReleaseCapture();
		m_pPlayer->m_pSkinnedAnimationController->m_bisRotate = false;
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	DWORD dwDirection = 0;

	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;

		case VK_RETURN:
			break;

		//case VK_F1:
		//case VK_F2:
		//case VK_F3:
		//case VK_F4:
		//	m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			//break;

		case VK_F9:
			ChangeSwapChainState();
			break;

		case 'P': // scene
		case 'O': // texture
		case 'Z': // depth
		case 'N': // normal
		{
			m_nDrawOption = (int)wParam;
			break;
		}

		case '0':
			// ������ȭ��
			SceneNum = 0;
			ReleaseObjects();
			BuildObjects(SceneNum);
			break;

		case '1':
			// �κ�ȭ��
			if (SceneNum == 0) {

				SceneNum = 1;
				isready = true;

				gNetwork.SendLoginfo();

				WaitForSingleObject(loginevent, INFINITE);

				cl_id = gNetwork.Getmyid();
				m_pPlayer->c_id = gNetwork.Getmyid();

				ReleaseObjects();
				BuildObjects(SceneNum);
				gNetwork.SendChangeScene(SceneNum);
				break;
			}
			else break;

		case '2':
			if (SceneNum == 0) break;
			// spaceship map
			SceneNum = 2;
			isready = false;
			// send ready packet  
			//if(처음 시작할 때에만 IngameStart() ) {}
			// bool 을 두면 간단 하지만? bool보단 그냥 클라마다 상태체크하는게 좋을거같긴함 Ingame상태이거나 게임중인 상태에는 보낼 필요가 없으니까? 
			// bool로 일단 해보자 
			if (gNetwork.ClientState == false)
			{
			gNetwork.SendIngameStart();
			}
			gNetwork.SendChangeScene(SceneNum);

			break;

		case '3':
			if (SceneNum == 1 || SceneNum == 0) break;
			// ice map
			SceneNum = 3;
			isready = false;
			ReleaseObjects();
			BuildObjects(SceneNum);
			gNetwork.SendChangeScene(SceneNum);
			break;

		case '4':
			if (SceneNum == 1 || SceneNum == 0) break;
			// fire map
			SceneNum = 4;
			ReleaseObjects();
			BuildObjects(SceneNum);
			gNetwork.SendChangeScene(SceneNum);
			break;

		case '5':
			if (SceneNum == 1 || SceneNum == 0) break;
			// grass map
			SceneNum = 5;
			ReleaseObjects();
			BuildObjects(SceneNum);
			gNetwork.SendChangeScene(SceneNum);
			break;

		case VK_SPACE:
			m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal = false;
			break;

		case '9':
			m_pPlayer->m_hp -= 5.0f;
			isready = true;
			break;

		case 'B':
			m_bRenderBoundingBox = !m_bRenderBoundingBox;
			break;

		default:
			break;
		}
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::myFunc_SetPosition(int n, int id, XMFLOAT3 position)
{
	if (cl_id == n)
	{
		m_pPlayer->SetId(cl_id);
		// ���� �Ǵ��� �ȵǴ��� �𸣰���
		//m_pPlayer->SetPosition(position);
	}
	else
	{
		int others_id = -1;
		switch (cl_id) {
		case 0:
			others_id = n - 1;
			break;
		case 1:
			others_id = n;
			if (n == 2) others_id = 1;
			break;
		case 2:
			others_id = n;
			break;
		}
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetPosition(position);
		//m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pChild->m_pChild->m_xmBoundingBox.Center = position;
	}
}




void CGameFramework::myFunc_SetLookRight(int n, int id, XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right)
{
	if (cl_id == n)
	{
		m_pPlayer->SetLook(Look);
		m_pPlayer->SetUp(Up);
		m_pPlayer->SetRight(Right);

	}
	else
	{
		int others_id = -1;
		switch (cl_id) {
		case 0:
			others_id = n - 1;
			break;
		case 1:
			others_id = n;
			if (n == 2) others_id = 1;
			break;
		case 2:
			others_id = n;
			break;
		}
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetLook(Look.x, Look.y, Look.z);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetUp(0, 1, 0);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetRight(Right.x, Right.y, Right.z);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetScale(20.0f, 20.0f, 20.0f);
	}
}

void CGameFramework::myFunc_SetMonPosition(int n, int SCSceneNum, XMFLOAT3 position)
{
	if (SCSceneNum > 1)
	{
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->isdraw = true;
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetPosition(position);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Center = position;
		//m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetScale(20, 20, 20);
	}
}

void CGameFramework::myFunc_SetMonLookRight(int n, int SCSceneNum, XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right)
{
	if (SCSceneNum > 1)
	{
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetLook(Look.x, Look.y, Look.z);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetUp(0, 1, 0);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetRight(Right.x, Right.y, Right.z);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetScale(20, 20, 20);

	}
}

void CGameFramework::myFunc_SetAnimation(int n, int id, int prevAni, int curAni)
{
	if (cl_id != n)
	{
		int others_id = -1;
		switch (cl_id) {
		case 0:
			others_id = n - 1;
			break;
		case 1:
			others_id = n;
			if (n == 2) others_id = 1;
			break;
		case 2:
			others_id = n;
			break;
		}

		if (id == 1 || id == 2)
		{
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(1, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(3, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(4, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(5, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(10, 0.5);
		}
		// �������� ���� ���� �ִϸ��̼� ��ȣ�� ���� �ִϸ��̼� ��ȣ�� �ٸ� ���(������ �ؾ��ϴ� ���)
		if (prevAni != curAni)
		{

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_bIsBlending = true;

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_nAnimationBefore = prevAni;
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_nAnimationAfter = curAni;

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackEnable(prevAni, false);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackEnable(curAni, true);

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackPosition(prevAni, 0.0f);

			//if ()
			//{

			//}
			g_clients[others_id + 1].setprevAnimation(curAni);
		}
	}
}

void CGameFramework::myFunc_SetAttack(int n, int id, bool isAttack)
{
	if (cl_id != n)
	{
		int others_id = -1;
		switch (cl_id) {
		case 0:
			others_id = n - 1;
			break;
		case 1:
			others_id = n;
			if (n == 2) others_id = 1;
			break;
		case 2:
			others_id = n;
			break;
		}

		if (isAttack == true)
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_bIsAttack = true;
	}
}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	//if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	//if (m_pPipelineState) m_pPipelineState->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

#define _WITH_TERRAIN_PLAYER

void CGameFramework::BuildObjects(int nScene)
{
	//m_pUILayer = UILayer::Create(m_nSwapChainBuffers, 0, m_pd3dDevice, m_pd3dCommandQueue, m_ppd3dSwapChainBackBuffers, m_nWndClientWidth, m_nWndClientHeight);

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	switch (nScene)
	{
	case 0:
	{
		m_pScene = new CLoginScene();
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

		CLoginPlayer* pPlayer = new CLoginPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

		m_pScene->m_pPlayer = m_pPlayer = pPlayer;
		m_pCamera = m_pPlayer->GetCamera();

		break;
	}
	case 1:
	{
		m_pScene = new CLobbyScene();
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);



		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain, 2);


		m_pScene->m_pPlayer = m_pPlayer = pPlayer;
		m_pCamera = m_pPlayer->GetCamera();


		break;
	}
	case 2:
	{
		m_pScene = new CSpaceShipScene();
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain, 1);

		m_pScene->m_pPlayer = m_pPlayer = pPlayer;
		m_pCamera = m_pPlayer->GetCamera();



		break;
	}
	case 3:
	{
		// ice map
		m_pScene = new CIceScene();
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

		m_pScene->m_pPlayer = m_pPlayer = pPlayer;
		m_pCamera = m_pPlayer->GetCamera();

		break;

	}
	case 4:
	{
		// fire map
		m_pScene = new CFireScene();
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

		m_pScene->m_pPlayer = m_pPlayer = pPlayer;
		m_pCamera = m_pPlayer->GetCamera();


		break;
	}

	case 5:
	{
		// grass map
		m_pScene = new CGrassScene();
		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


		CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

		m_pScene->m_pPlayer = m_pPlayer = pPlayer;
		m_pCamera = m_pPlayer->GetCamera();


		break;
	}
	default:
		break;
	}
	m_pPostProcessingShader = new CTextureToFullScreenShader();
	m_pPostProcessingShader->CreateShader(m_pd3dDevice, m_pScene->GetGraphicsRootSignature(), 1, NULL, DXGI_FORMAT_D32_FLOAT);
	m_pPostProcessingShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, &m_nDrawOption);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * m_nSwapChainBuffers);

	DXGI_FORMAT pdxgiResourceFormats[5] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	m_pPostProcessingShader->CreateResourcesAndRtvsSrvs(m_pd3dDevice, m_pd3dCommandList, 5, pdxgiResourceFormats, d3dRtvCPUDescriptorHandle); //SRV to (Render Targets) + (Depth Buffer)

	// ���� SRV ��¼��..
	D3D12_GPU_DESCRIPTOR_HANDLE d3dDsvGPUDescriptorHandle = CScene::CreateShaderResourceView(m_pd3dDevice, m_pd3dDepthStencilBuffer, DXGI_FORMAT_R32_FLOAT);


	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	/*  if (m_pUILayer) m_pUILayer->ReleaseResources();
	  if (m_pUILayer) delete m_pUILayer;*/

	if (m_pScene->m_pPlayer) m_pScene->m_pPlayer->Release();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) {
		delete m_pScene;
		m_pScene = nullptr;
	}

	if (m_pPostProcessingShader) m_pPostProcessingShader->ReleaseObjects();
	if (m_pPostProcessingShader) m_pPostProcessingShader->ReleaseShaderVariables();
	if (m_pPostProcessingShader) m_pPostProcessingShader->Release();
	m_pPostProcessingShader = nullptr;
}

void CGameFramework::ProcessInput()
{
	m_pPlayer->m_xmf3BeforeCollidedPosition = m_pPlayer->GetPosition();

	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene && (SceneNum != 0))
	{
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		DWORD dwDirection = 0;

		if (pKeysBuffer[KEY_W] & 0xF0 && m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal == false)
			dwDirection |= DIR_FORWARD;
		if (pKeysBuffer[KEY_S] & 0xF0 && m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal == false)
			dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer[KEY_A] & 0xF0 && m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal == false)
			dwDirection |= DIR_LEFT;
		if (pKeysBuffer[KEY_D] & 0xF0 && m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal == false)
			dwDirection |= DIR_RIGHT;

		if (pKeysBuffer[VK_F1] & 0xF0)
		{
			m_pPlayer->m_pSkinnedAnimationController->m_bIsDead = true;
			m_pPlayer->m_pSkinnedAnimationController->m_bIsBlending = true;
		}
		if (pKeysBuffer[VK_F2] & 0xF0)
		{
			m_pPlayer->m_pSkinnedAnimationController->m_bIsDead = false;
			m_pPlayer->m_pSkinnedAnimationController->m_bIsLastBlending = true;
		}

		if (pKeysBuffer[VK_SPACE] & 0xF0)
			m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal = true;

		// 공격키
		if (pKeysBuffer[VK_LBUTTON] & 0xF0
			&& m_pPlayer->m_pSkinnedAnimationController->m_bIsDead == false)
		{
			m_pPlayer->m_pSkinnedAnimationController->m_bIsAttack = true;
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0) {
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
					g_clients[cl_id].setLook(m_pPlayer->GetLook());
					g_clients[cl_id].setRight(m_pPlayer->GetRight());
					g_clients[cl_id].setUp(m_pPlayer->GetUp());
					g_sendqueue.push(SENDTYPE::ROTATE);
				}
			}

			if (dwDirection && m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal == false)
			{
				m_pPlayer->m_pSkinnedAnimationController->m_bIsMove = true;
				m_pPlayer->Move(dwDirection, 12.25f, true);
				XMFLOAT3 exveloctiy = m_pPlayer->GetVelocity();
				XMFLOAT3 exGravity = m_pPlayer->GetGravity();
				XMFLOAT3 temp = Vector3::Add(exveloctiy, { 0.0f,0.0f,0.f });
				float fLength = sqrtf(temp.x * temp.x + temp.z * temp.z);
				float fMaxVelocityXZ = m_pPlayer->GetMaxVelocityXZ();// m_fMaxVelocityXZ;
				if (fLength > m_pPlayer->GetMaxVelocityXZ())
				{
					temp.x *= (fMaxVelocityXZ / fLength);
					temp.z *= (fMaxVelocityXZ / fLength);
				}
				float fMaxVelocityY = m_pPlayer->GetMaxVelocityY();
				fLength = sqrtf(temp.y * temp.y);
				if (fLength > m_pPlayer->GetMaxVelocityY()) temp.y *= (fMaxVelocityY / fLength);

				XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(temp, m_GameTimer.GetTimeElapsed(), false);
				m_pPlayer->Move(xmf3Velocity, false);
				g_clients[cl_id].setPos(m_pPlayer->GetPosition());
				g_sendqueue.push(SENDTYPE::MOVE);

			}
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	//m_pPlayer->UpdateBoundingBox();
	if (m_pScene) m_pScene->AnimateObjects(fTimeElapsed);

	m_pPlayer->Animate(fTimeElapsed);

	if (SceneNum > 0 && m_pScene->CheckObjectByObjectCollisions(m_pPlayer))
	{
		// ���⼭ �� ��ȯ ó�����ָ� �� ��
		//if (m_pPlayer->isFireMap == true)
		//{
		//	m_pPlayer->isFireMap = false;
		//	// ����ȯ
		//}
		//else if (m_pPlayer->isGrassMap == true)
		//{
		//	m_pPlayer->isGrassMap = false;
		//	// ����ȯ
		//}
		//else if (m_pPlayer->isIceMap == true)
		//{
		//	m_pPlayer->isIceMap = false;
		//	// ����ȯ
		//}
		//else
		m_pPlayer->SetPosition(m_pPlayer->m_xmf3BeforeCollidedPosition);
	}
}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::UpdateUI()
{
	total++;
	if (total % 10 == 0) {

		curSecond++;
		if (curSecond == 60) {
			curSecond = 0;
			curMinute++;
			curMinute = curMinute % 5;
			curDay++;
		}
	}
	// �ð�
	//id2d1solidcolorbrush* pd2dbrush = m_puilayer->createbrush(d2d1::colorf(d2d1::colorf::pink, 1.0f));
	//idwritetextformat* pdwtextformat = m_puilayer->createtextformat(l"���� ����", m_nwndclientheight / 15.0f);
	//d2d1_rect_f d2drect = d2d1::rectf(00.0f, 0.0f, (float)m_nwndclientwidth, (float)m_nwndclientheight);

	//wchar pstroutputtext[256];
	//swprintf_s(pstroutputtext, 256, l"day: %d  time:%02d:%02d", curday, curminute, cursecond);
	//m_puilayer->updatetextoutputs(0, pstroutputtext, &d2drect, pdwtextformat, pd2dbrush);

	// ��������
	// ü�¹�
   /* float rectwidth = (m_pplayer->m_hp / 100.0f) * 2.0f * 20.0f;

	d2d1_rect_f rect = { 400.0f, 430.0f, 400 + rectwidth * 5.0, (float)m_nwndclientheight - 20.0 };
	id2d1solidcolorbrush* redbrush = m_puilayer->createbrush(d2d1::colorf(d2d1::colorf::red, 1.0f));

	m_puilayer->updatetextoutputs(1, null, &rect, null, redbrush);*/

}


//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(60.0f);

	ProcessInput();

	AnimateObjects();

	//if(SceneNum > 1)
	//	UpdateUI();


	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pScene->OnPrepareRender(m_pd3dCommandList, m_pCamera);

	//if (m_nDrawOption == DRAW_SCENE_COLOR)//'S'
	{

		m_pd3dCommandList->ClearDepthStencilView(m_d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

		m_pPostProcessingShader->OnPrepareRenderTarget(m_pd3dCommandList, 1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], &m_d3dDsvDescriptorCPUHandle);

		m_pScene->Render(m_pd3dCommandList, m_pCamera);

		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

		if (m_bRenderBoundingBox) m_pScene->RenderBoundingBox(m_pd3dCommandList, m_pCamera);

		m_pPostProcessingShader->OnPostRenderTarget(m_pd3dCommandList);
	}
	//else
	{
		// �ĸ� ���ۿ� �ش��ϴ°�
		m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], TRUE, NULL);

		m_pPostProcessingShader->Render(m_pd3dCommandList, m_pCamera, &m_nDrawOption);

		//m_pScene->Render(m_pd3dCommandList, m_pCamera);

	}

	::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	//if (m_pUILayer)
	//{
	//	UILayer::GetInstance()->Render(m_nSwapChainBufferIndex, SceneNum, isready, curDay, curMinute, curSecond);
	//}



#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	size_t nLength = _tcslen(m_pszFrameRate);
	XMFLOAT3 xmf3Position = m_pPlayer->GetPosition();
	_stprintf_s(m_pszFrameRate + nLength, 70 - nLength, _T("(%4f, %4f, %4f)"), xmf3Position.x, xmf3Position.y, xmf3Position.z);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

