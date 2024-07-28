//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "UI.h"


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
	m_pd3dImGuiDescriptorHeap = NULL;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = new CScene;
	m_pPlayer = new CPlayer;
	m_pTime = new TIME;
	m_pCamera = new CCamera;
	m_pUILayer = new UILayer;

	m_hInstance = nullptr;
	m_hWnd = nullptr;
	m_ptOldCursorPos = {};

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
	CreateRtvAndDsvAndImGuiDescriptorHeaps();
	
	CreateSwapChain();

	CreateSwapChainRenderTargetViews();

	CreateDepthStencilView();

	//HRESULT res = CoInitialize(NULL);

	InitXAudio2();

	CreateShadowMapCamera();
	BuildObjects(sceneManager.GetCurrentScene());
	CreateShadowMap();

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
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

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

void CGameFramework::CreateRtvAndDsvAndImGuiDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers + 5;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
	if (SUCCEEDED(hResult))
	{
		m_pd3dRtvDescriptorHeap->SetName(L"CGameFramework::CreateRtvAndDsvDescriptorHeaps 1");
	}
	// **
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 2;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
	if (SUCCEEDED(hResult))
	{
		m_pd3dDsvDescriptorHeap->SetName(L"CGameFramework::CreateRtvAndDsvDescriptorHeaps 2");
	}
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//imgui
	//d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//d3dDescriptorHeapDesc.NumDescriptors = 1;
	//d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//d3dDescriptorHeapDesc.NodeMask = 0;
	//hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dImGuiDescriptorHeap);
	//if (SUCCEEDED(hResult))
	//{
	//	m_pd3dImGuiDescriptorHeap->SetName(L"ImGUIDescriptorHeaps");
	//}
	//::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE m_DSVDescriptorCPUHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, m_DSVDescriptorCPUHandle);
}
void CGameFramework::CreateShadowMap()
{
	m_ShadowMap = make_unique<ShadowMap>(m_pd3dDevice, 2048, 2048);
	//m_ShadowMap = make_unique<ShadowMap>(m_pd3dDevice, 4096, 4096);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dDsvCPUDescriptorHandle.ptr += (::gnDsvDescriptorIncrementSize);

	/// 

	m_ShadowMap->BuildDescriptors(m_pPostProcessingShader->GetCPUSrvDescriptorNextHandle(), m_pPostProcessingShader->GetGPUSrvDescriptorNextHandle(), d3dDsvCPUDescriptorHandle);

	m_ShadowMap->CreateShader(m_pd3dDevice, m_pScene->GetGraphicsRootSignature(), 1, NULL, DXGI_FORMAT_D32_FLOAT);

}

void CGameFramework::CreateShadowMapCamera()
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256의 배수
	m_pShadowCamera = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pShadowCamera->Map(0, NULL, (void**)&m_pShadowMappedCamera);
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

	if (m_pBlurBuffer) m_pBlurBuffer->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateSwapChainRenderTargetViews();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		if (sceneManager.GetCurrentScene() == SCENEKIND::LOGIN && FRAME_BUFFER_HEIGHT > 1000)
		{
			UILayer::GetInstance()->ProcessMouseClick(SCENEKIND::LOGIN, m_ptOldCursorPos);
		}
		// 플레이어의 m_bIsDead가 true면 공격 패킷 보내면 안됨!!!!!!
		if (g_clients.size() == 0)break;
		g_clients[cl_id].setAttack(true);
		g_sendqueue.push(SENDTYPE::ATTACK);


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
	if (m_pScene)
	{
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	}

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
			//SceneNum = 0;
			sceneManager.SetCurrentScene(SCENEKIND::LOGIN);
			ReleaseObjects();
			BuildObjects(sceneManager.GetCurrentScene());
			break;

		case '1':
			// �κ�ȭ��
			if (sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) {
				SceneNum = 1;
				isSceneChange = true;
				isready = false;


				break;
			}
			break;

		case '2': {
			if (sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
			// spaceship map
			SceneNum = 2;
			isready = true;
			isSceneChange = true;
		}
				break;

		case '3':
			if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY || sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
			// ice map
			SceneNum = 3;
			//isSceneChange = true;
			isSceneChangetoIce = true;
			//sceneManager.SetCurrentScene(SCENEKIND::ICE);
			isready = false;
			//ReleaseObjects();
			//BuildObjects(sceneManager.GetCurrentScene());


			break;

		case '4':
			if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY || sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
			// fire map
			SceneNum = 4;
			//sceneManager.SetCurrentScene(SCENEKIND::FIRE);
			//ReleaseObjects();
			//BuildObjects(sceneManager.GetCurrentScene());
			//isSceneChange = true;
			isSceneChangetoFire = true;
			//g_sendqueue.push(SENDTYPE::CHANGE_STAGE);

			break;

		case '5':
			if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY || sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
			// grass map
			SceneNum = 5;
			/*	sceneManager.SetCurrentScene(SCENEKIND::NATURE);
				ReleaseObjects();
				BuildObjects(sceneManager.GetCurrentScene());*/
				//isSceneChange = true;
			isSceneChangetoNature = true;
			//g_sendqueue.push(SENDTYPE::CHANGE_STAGE);
			break;

		case VK_SPACE:
			m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal = false;
			break;

		case '9':
			m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 5.0f);
			isready = true;
			break;

		case 'B':
			//m_bRenderBoundingBox = !m_bRenderBoundingBox;
			//isSceneChange = true;
			g_sendqueue.push(SENDTYPE::CHANGE_SCENE_INGAME_START);
			//g_sendqueue.push(SENDTYPE::CHANGE_STAGE);
			break;
		case 'L':
			m_pScene->m_ppHierarchicalGameObjects[13]->SetHealth(-100);
			break;
		case 'M':
			isWin = true;
			break;
		case 'K':
			isLose = true;
			break;
		case 'F':
			// 맵 이동 관련
			PlayerPosX = m_pPlayer->GetPosition().x;
			PlayerPosZ = m_pPlayer->GetPosition().z;
			if (PlayerPosX > 0 && PlayerPosZ > 0)
			{
				if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY || sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
				// ice map
				SceneNum = 3;
				isSceneChangetoIce = true;
				isready = false;

				break;
			}
			else if (PlayerPosX > 0 && PlayerPosZ < 0)
			{
				if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY || sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
				// fire map
				SceneNum = 4;
				isSceneChangetoFire = true;
				isready = false;
				break;
			}
			else if (PlayerPosX < 0 && PlayerPosZ > 0)
			{
				if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY || sceneManager.GetCurrentScene() == SCENEKIND::LOGIN) break;
				// grass map
				SceneNum = 5;
				isSceneChangetoNature = true;
				isready = false;
				break;
			}
			else
				break;
			break;

		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F8:
			isBlurRender = !isBlurRender;
			//ChangeSwapChainState();
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
	break;
	case WM_SIZE:
		break;
	case WM_LBUTTONDOWN:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
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

void CGameFramework::InitXAudio2()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
		std::cerr << "CoInitializeEx 실패: " << std::hex << hr << std::endl;
		exit(1);
	}

	HRESULT hResult = XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hResult)) {
		std::cerr << "XAudio2 초기화 실패: " << std::hex << hResult << std::endl;
		exit(1);
	}
	hResult = m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice);
	if (FAILED(hResult)) {
		std::cerr << "Failed to create mastering voice" << std::endl;
	}
}

void CGameFramework::CleanupXAudio2() {
	if (m_pMasterVoice) m_pMasterVoice->DestroyVoice();
	if (m_pXAudio2) m_pXAudio2->Release();
	CoUninitialize();
}

SoundData CGameFramework::LoadWaveFile(const wchar_t* filename) {
	SoundData soundData = {};
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		std::cerr << "Failed to open sound file" << std::endl;
		exit(1);
	}

	// RIFF 헤더를 읽습니다.
	char riffHeader[4];
	file.read(riffHeader, 4);
	if (strncmp(riffHeader, "RIFF", 4) != 0) {
		std::cerr << "Invalid WAV file: RIFF header not found" << std::endl;
		exit(1);
	}

	// 파일 크기를 건너뜁니다.
	file.seekg(4, std::ios::cur);

	// WAVE 헤더를 읽습니다.
	char waveHeader[4];
	file.read(waveHeader, 4);
	if (strncmp(waveHeader, "WAVE", 4) != 0) {
		std::cerr << "Invalid WAV file: WAVE header not found" << std::endl;
		exit(1);
	}

	// 서브 청크들을 순회하면서 fmt 청크와 data 청크를 찾습니다.
	while (file.good()) {
		char chunkHeader[4];
		DWORD chunkSize;
		file.read(chunkHeader, 4);
		file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));

		if (strncmp(chunkHeader, "fmt ", 4) == 0) {
			if (chunkSize == 16 || chunkSize == 18) {
				file.read(reinterpret_cast<char*>(&soundData.wfx), 16); // WAVEFORMAT 구조체 크기
				if (chunkSize > 16) {
					file.read(reinterpret_cast<char*>(&soundData.wfx.cbSize), 2); // 추가 크기 읽기
				}
				if (chunkSize > 18) {
					file.seekg(chunkSize - 18, std::ios::cur); // 나머지 건너뛰기
				}
			}
			else {
				std::cerr << "Unexpected fmt chunk size" << std::endl;
				exit(1);
			}
		}
		else if (strncmp(chunkHeader, "data", 4) == 0) {
			soundData.pData = new BYTE[chunkSize];
			file.read(reinterpret_cast<char*>(soundData.pData), chunkSize);
			soundData.buffer.AudioBytes = chunkSize;
			soundData.buffer.pAudioData = soundData.pData;
			soundData.buffer.Flags = XAUDIO2_END_OF_STREAM;
			break;
		}
		else {
			// 다른 서브 청크를 건너뜁니다.
			file.seekg(chunkSize, std::ios::cur);
		}
	}

	if (soundData.buffer.AudioBytes == 0) {
		std::cerr << "Failed to find data chunk" << std::endl;
		exit(1);
	}

	return soundData;
}

void CGameFramework::PlayAttackSound(IXAudio2* pXAudio2, const SoundData& soundData) {
	IXAudio2SourceVoice* pSourceVoice;
	HRESULT hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfx);
	if (SUCCEEDED(hr)) {
		pSourceVoice->SubmitSourceBuffer(&soundData.buffer);
		pSourceVoice->Start(0);
	}
	else {
		std::cerr << "Failed to create source voice: " << std::hex << hr << std::endl;
	}
}

void CGameFramework::PlaySounds(IXAudio2* pXAudio2, const SoundData& soundData) {
	if (pXAudio2) {
		HRESULT hr = pXAudio2->CreateSourceVoice(&m_pSourceVoice, &soundData.wfx);
		if (SUCCEEDED(hr)) {
			m_pSourceVoice->SubmitSourceBuffer(&soundData.buffer);
			m_pSourceVoice->Start(0);
		}
		else {
			std::cerr << "Failed to create source voice: " << std::hex << hr << std::endl;
		}
	}
	else {
		std::cerr << "pXAudio2 is null" << std::endl;
	}
}

void CGameFramework::ChangeBGM(int nSceneKind)
{
	if (m_pSourceVoice) {
		m_pSourceVoice->Stop(0);
		m_pSourceVoice->DestroyVoice();
	}

	m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, &m_SceneSounds[nSceneKind].wfx);
	m_pSourceVoice->SubmitSourceBuffer(&m_SceneSounds[nSceneKind].buffer);
	m_pSourceVoice->Start(0);
}

void CGameFramework::ChangeScene(SCENEKIND nSceneKind)
{

	if (nSceneKind != sceneManager.GetCurrentScene())
	{
		if (m_pPlayer)
			ChangeSceneReleaseObject();

		switch (nSceneKind)
		{

		case SCENEKIND::LOBBY:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}

			sceneManager.SetCurrentScene(nSceneKind);
			isSceneChange = false;
			cout << "CLobbyScene BuildObjects" << endl;
			m_pScene = new CLobbyScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

			CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain, 2);


			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();

			break;
		}
		case SCENEKIND::SPACESHIP:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}
			isSceneChange = false;
			sceneManager.SetCurrentScene(nSceneKind);
			cout << "CSpaceShipScene BuildObjects" << endl;
			//this_thread::sleep_for(10ms);
			m_pScene = new CSpaceShipScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

			CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain, 1);

			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();

			//SoundData SpaceshipBgm = LoadWaveFile(L"Sound/Day.wav");
			//ChangeBGM(0);

			break;
		}
		case SCENEKIND::ICE:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}

			isSceneChangetoIce = false;
			sceneManager.SetCurrentScene(nSceneKind);
			cout << "CIceScene BuildObjects" << endl;
			// ice map
			m_pScene = new CIceScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


			CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();

			//ChangeBGM(1);

			break;

		}
		case SCENEKIND::FIRE:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}
			isSceneChangetoFire = false;
			sceneManager.SetCurrentScene(nSceneKind);
			// fire map
			m_pScene = new CFireScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


			CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();

			//ChangeBGM(2);

			break;
		}

		case SCENEKIND::NATURE:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}
			isSceneChangetoNature = false;

			sceneManager.SetCurrentScene(nSceneKind);
			// grass map
			m_pScene = new CGrassScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


			CTerrainPlayer* pPlayer = new CTerrainPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();

			//ChangeBGM(3);

			break;
		}
		case SCENEKIND::VICTORY:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}
			isWin = false;

			sceneManager.SetCurrentScene(nSceneKind);
			// victory map
			m_pScene = new CWInScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


			CLoginPlayer* pPlayer = new CLoginPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();


			break;
		}
		case SCENEKIND::DEFEAT:
		{
			if (S_OK != m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL)) {
				cout << "CommandList Reset Fail" << endl;
			}
			isLose = false;

			sceneManager.SetCurrentScene(nSceneKind);
			// Lose map
			m_pScene = new CLoseScene();
			m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);


			CLoginPlayer* pPlayer = new CLoginPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

			m_pScene->m_pPlayer = m_pPlayer = pPlayer;
			m_pCamera = m_pPlayer->GetCamera();


			break;
		}
		default:
			break;
		}


		//m_pPostProcessingShader = new CTextureToFullScreenShader();
		//m_pPostProcessingShader->CreateShader(m_pd3dDevice, m_pScene->GetGraphicsRootSignature(), 1, NULL, DXGI_FORMAT_D32_FLOAT);
		//m_pPostProcessingShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, &m_nDrawOption);

		//D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		//d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * m_nSwapChainBuffers);

		//DXGI_FORMAT pdxgiResourceFormats[5] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
		//m_pPostProcessingShader->CreateResourcesAndRtvsSrvs(m_pd3dDevice, m_pd3dCommandList, 5, pdxgiResourceFormats, d3dRtvCPUDescriptorHandle, 6); //SRV to (Render Targets) + (Depth Buffer)

		//// ���� SRV ��¼��..
		////D3D12_GPU_DESCRIPTOR_HANDLE d3dDsvGPUDescriptorHandle = CScene::CreateShaderResourceView(m_pd3dDevice, m_pd3dDepthStencilBuffer, DXGI_FORMAT_R32_FLOAT);

		//DXGI_FORMAT pdxgiDepthSrvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
		//m_pPostProcessingShader->CreateShaderResourceViews(m_pd3dDevice, 1, &m_pd3dDepthStencilBuffer, pdxgiDepthSrvFormats);

		//CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
		//m_pBlurBuffer = pTexture->CreateTexture(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, NULL, RESOURCE_TEXTURE2D, 0, 1);
		//m_pBlurBuffer->AddRef();

		//m_BlurShader = make_unique<CBlurShader>();
		//m_BlurShader->CreateShader(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
		//m_BlurShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), NULL, pTexture);

		m_pd3dCommandList->Close();
		ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
		m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

		WaitForGpuComplete();

		if (m_pScene) m_pScene->ReleaseUploadBuffers();
		if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

		m_GameTimer.Reset();
	}
}

void CGameFramework::myFunc_SetPosition(int n, int id, XMFLOAT3 position)
{
	if (cl_id == n)
	{
		m_pPlayer->SetId(cl_id);
		m_pPlayer->SetPosition(position);
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

void CGameFramework::myFunc_SetLookRightUp(int n, int id, XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right)
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
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetUp(Up.x, Up.y, Up.z);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetRight(Right.x, Right.y, Right.z);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->SetScale(20.0f, 20.0f, 20.0f);
	}
}

void CGameFramework::myFunc_SetMonPosition(int n, XMFLOAT3 position)
{
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->isdraw = true;
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetPosition(position);
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Center = position;
	//m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetScale(20, 20, 20);
}

void CGameFramework::myFunc_SetBossMonPosition(XMFLOAT3 position)
{
	m_pScene->m_ppHierarchicalGameObjects[13]->isdraw = true;
	m_pScene->m_ppHierarchicalGameObjects[13]->SetPosition(position);
	m_pScene->m_ppHierarchicalGameObjects[13]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Center = position;
	//m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetScale(20, 20, 20);
}

void CGameFramework::myFunc_SetMonLookRightUp(int n, XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right)
{
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetLook(Look.x, Look.y, Look.z);
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetUp(0, 1, 0);
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetRight(Right.x, Right.y, Right.z);
	m_pScene->m_ppHierarchicalGameObjects[n + 3]->SetScale(20, 20, 20);

}

void CGameFramework::myFunc_SetBossMonLookRightUp(XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right)
{
	m_pScene->m_ppHierarchicalGameObjects[13]->SetLook(Look.x, Look.y, Look.z);
	m_pScene->m_ppHierarchicalGameObjects[13]->SetUp(0, 1, 0);
	m_pScene->m_ppHierarchicalGameObjects[13]->SetRight(Right.x, Right.y, Right.z);
	m_pScene->m_ppHierarchicalGameObjects[13]->SetScale(20, 20, 20);

}

void CGameFramework::myFunc_SetAnimation(int n, int id, animateState prevAni, animateState curAni)
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

		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 1.5);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 1.5);

		if (prevAni != curAni)
		{
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_bIsBlending = true;

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_nAnimationBefore = int(prevAni);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_nAnimationAfter = int(curAni);

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackEnable(int(prevAni), false);
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackEnable(int(curAni), true);

			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackPosition(int(prevAni), 0.0f);


			g_clients[id].setprevAnimation(curAni);
		}
	}
}

void CGameFramework::myFunc_SetMonAnimation(int n, bool isAttacked, bool isAttack)
{
	if (isAttacked == true)
	{
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->m_bIsAttacked = true;
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(3, true);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	}
	else if (isAttack == true)
	{
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->m_bIsMonsterAttack = true;
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->m_bWasMonsterAttack = true;
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(4, true);
	}
	else if (isAttack == false && m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->m_bWasMonsterAttack)
	{
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->m_bIsMonsterAttack = false;
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->m_bWasMonsterAttack = false;
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
		m_pScene->m_ppHierarchicalGameObjects[n + 3]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
	}
}

void CGameFramework::myFunc_SetBossMonAnimation(bool isAttacked, bool isAttack, int attackAniNum)
{
	if (isAttacked == true)
	{
		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_bIsAttacked = true;

		for (int i = 0; i < m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_nAnimationTracks; i++)
			m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(i, false);

		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(3, true);
	}
	else if (isAttack == true)
	{
		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_bIsMonsterAttack = true;
		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_bWasMonsterAttack = true;

		for (int i = 0; i < m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_nAnimationTracks; i++)
			m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(i, false);

		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(attackAniNum, true);
	}
	else if (isAttack == false && m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_bWasMonsterAttack)
	{
		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_bIsMonsterAttack = false;
		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_bWasMonsterAttack = false;

		for (int i = 0; i < m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->m_nAnimationTracks; i++)
			m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(i, false);

		m_pScene->m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
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

		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 1.0);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 1.0);

		if (isAttack == true)
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_bIsAttack = true;
	}
}

void CGameFramework::myFunc_SetAttacked(int n, int id, bool isAttacked)
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

		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 1.0);
		m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 1.0);

		if (isAttacked == true)
			m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
	}
}

void CGameFramework::myFunc_SetBlind(int n, int id, bool _isblind)
{
	if (cl_id == n)
	{
		m_pPlayer->SetId(cl_id);
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
		if (m_pScene->m_ppHierarchicalGameObjects[others_id + 1])m_pScene->m_ppHierarchicalGameObjects[others_id + 1]->isdraw = _isblind;
	}

}

void CGameFramework::myFunc_SetStatus(int FireCnt, int IceCnt, int NatureCnt)
{
	curDay++;

	int attack = g_clients[cl_id].getAttackPower() + (FireCnt * 5);
	
	if (attack > 100) {
		attack = 100;
	}
	//m_pPlayer->SetAttackPower(attack);
	g_clients[cl_id].setAttackPower(attack);

	int speed = m_pPlayer->GetSpeed() + (IceCnt * 3);
	m_pPlayer->SetSpeed(speed);

	int health = m_pPlayer->GetHealth() + (NatureCnt * 5);
	m_pPlayer->SetHealth(health);
}



void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();


	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	//if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	//if (m_pPipelineState) m_pPipelineState->Release();

	if (m_pShadowCamera)
	{
		m_pShadowCamera->Release();
		m_pShadowCamera = nullptr;
	}
	if (m_pShadowMappedCamera)
	{
		//delete m_pShadowMappedCamera;
		m_pShadowMappedCamera = nullptr;
	}

	if (m_pBlurBuffer)m_pBlurBuffer->Release();

	if (m_pd3dcbTime)
	{
		m_pd3dcbTime->Unmap(0, NULL);
		m_pd3dcbTime->Release();
	}

	//imgui
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();

#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

#define _WITH_TERRAIN_PLAYER

void CGameFramework::BuildObjects(SCENEKIND m_nCurScene)
{
#ifdef _FULLSCREEN
	m_pUILayer = UILayer::Create(m_nSwapChainBuffers, 0, m_pd3dDevice, m_pd3dCommandQueue, m_ppd3dSwapChainBackBuffers, m_nWndClientWidth, m_nWndClientHeight);

#endif // _FULLSCREEN



	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	cout << "CLoginScene BuildObjects" << endl;
	m_pScene = new CLoginScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	CLoginPlayer* pPlayer = new CLoginPlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->m_pTerrain);

	m_pScene->m_pPlayer = m_pPlayer = pPlayer;
	m_pCamera = m_pPlayer->GetCamera();

	m_SceneSounds[0] = LoadWaveFile(L"Sound/Day.wav");
	m_SceneSounds[1] = LoadWaveFile(L"Sound/Ice.wav");
	m_SceneSounds[2] = LoadWaveFile(L"Sound/Fire.wav");
	m_SceneSounds[3] = LoadWaveFile(L"Sound/Grass.wav");

		// bgm
	//SoundData IceBgm = m_pScene->LoadWaveFile(L"Scene2BGM.wav");
	//SoundData FireBgm = m_pScene->LoadWaveFile(L"Scene3BGM.wav");
	//SoundData GrassBgm = m_pScene->LoadWaveFile(L"Scene4BGM.wav");

	CreateShaderVariables();

	m_pPostProcessingShader = new CTextureToFullScreenShader();
	m_pPostProcessingShader->CreateShader(m_pd3dDevice, m_pScene->GetGraphicsRootSignature(), 0, NULL, DXGI_FORMAT_UNKNOWN);
	m_pPostProcessingShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, &m_nDrawOption);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (::gnRtvDescriptorIncrementSize * m_nSwapChainBuffers);

	DXGI_FORMAT pdxgiResourceFormats[5] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
	m_pPostProcessingShader->CreateResourcesAndRtvsSrvs(m_pd3dDevice, m_pd3dCommandList, 5, pdxgiResourceFormats, d3dRtvCPUDescriptorHandle, 8); //SRV to (Render Targets) + (Depth Buffer)

	// ���� SRV ��¼��..
	//D3D12_GPU_DESCRIPTOR_HANDLE d3dDsvGPUDescriptorHandle = CScene::CreateShaderResourceView(m_pd3dDevice, m_pd3dDepthStencilBuffer, DXGI_FORMAT_R32_FLOAT);

	DXGI_FORMAT pdxgiDepthSrvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pPostProcessingShader->CreateShaderResourceViews(m_pd3dDevice, 1, &m_pd3dDepthStencilBuffer, pdxgiDepthSrvFormats);

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	m_pBlurBuffer = pTexture->CreateTexture(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON, NULL, RESOURCE_TEXTURE2D, 0, 1);
	m_pBlurBuffer->AddRef();

	m_BlurShader = make_unique<CBlurShader>();
	m_BlurShader->CreateShader(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature());
	m_BlurShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), NULL, pTexture);

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
#ifdef _FULLSCRREN
	if (m_pUILayer) m_pUILayer->ReleaseResources();
	if (m_pUILayer) delete m_pUILayer;

#endif // _FULLSCRREN

	//if (m_pScene->m_pPlayer) m_pScene->m_pPlayer->Release();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) {
		delete m_pScene;
		m_pScene = nullptr;
	}
	if (m_pPlayer) m_pPlayer->Release();

	if (m_pPostProcessingShader) m_pPostProcessingShader->ReleaseObjects();
	if (m_pPostProcessingShader) m_pPostProcessingShader->ReleaseShaderVariables();
	if (m_pPostProcessingShader) m_pPostProcessingShader->Release();
	m_pPostProcessingShader = nullptr;
}

void CGameFramework::ChangeSceneReleaseObject()
{
	if (m_pPlayer) {
		m_pPlayer->Release();
	}

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) {
		delete m_pScene;
		m_pScene = nullptr;
	}
}

void CGameFramework::ProcessInput()
{
	m_pPlayer->m_xmf3BeforeCollidedPosition = m_pPlayer->GetPosition();

	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene && sceneManager.GetCurrentScene() != SCENEKIND::LOGIN)
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

		//if (pKeysBuffer[VK_F1] & 0xF0)
		//{
		//	m_pPlayer->m_pSkinnedAnimationController->m_bIsDead = true;
		//	m_pPlayer->m_pSkinnedAnimationController->m_bIsBlending = true;
		//}
		//if (pKeysBuffer[VK_F2] & 0xF0)
		//{
		//	m_pPlayer->m_pSkinnedAnimationController->m_bIsDead = false;
		//	m_pPlayer->m_pSkinnedAnimationController->m_bIsLastBlending = true;
		//}

		if (pKeysBuffer[VK_SPACE] & 0xF0
			&& m_pPlayer->m_pSkinnedAnimationController->m_bIsDead == false)
			m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal = true;

		// 공격키
		if (pKeysBuffer[VK_LBUTTON] & 0xF0
			&& m_pPlayer->m_pSkinnedAnimationController->m_bIsDead == false)
		{
			m_pPlayer->m_pSkinnedAnimationController->m_bIsAttack = true;
			PlaySound(L"Sound/Attack.wav", NULL, SND_FILENAME);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0
					&& m_pPlayer->m_pSkinnedAnimationController->m_bIsDead == false) {

					float yaw = cxDelta;

					if (yaw > 360.0f) yaw -= 360.0f;
					if (yaw < 0.f) yaw += 360.0f;

					m_pPlayer->RotateYaw(yaw);
					if (g_clients.size() != 0)
					{
						g_clients[gNetwork.Getmyid()].m_yaw = yaw;
						g_sendqueue.push(SENDTYPE::ROTATE);
					}
				}
			}

			if (dwDirection && m_pPlayer->m_pSkinnedAnimationController->m_bIsHeal == false
				&& m_pPlayer->m_pSkinnedAnimationController->m_bIsDead == false)
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
				m_pPlayer->Move(cl_id, xmf3Velocity, false);
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

	if (sceneManager.GetCurrentScene() != SCENEKIND::LOGIN && m_pScene->CheckObjectByObjectCollisions() && m_pScene)
		m_pPlayer->SetPosition(m_pPlayer->m_xmf3BeforeCollidedPosition);
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

void CGameFramework::CreateShaderVariables()
{
	UINT ncbElementBytes = ((sizeof(UINT) + 255) & ~255); //256의 배수
	m_pd3dcbTime = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbTime->Map(0, NULL, (void**)&m_pTime);
}

void CGameFramework::UpdateShaderVariables()
{

	TIME temp;
	temp.fCurrentMin = curMinute;
	temp.fCurrentSec = curSecond;

	::memcpy(m_pTime, &temp, sizeof(TIME));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbTime->GetGPUVirtualAddress();
	m_pd3dCommandList->SetGraphicsRootConstantBufferView(16, d3dcbLightsGpuVirtualAddress);


}

void CGameFramework::UpdateTime()
{
	// 서버에서 제공하는 낮과 밤의 시간 (초 단위)
	float serverDayTime = 60.0f;  // 낮 시간 (예: 60초)
	float serverNightTime = 30.0f;  // 밤 시간 (예: 30초)

	// 클라이언트에서 관리하는 누적 시간 변수 초기화
	static float accumulatedTime = 0.0f;  // 현재 누적 시간
	static int curDay = 0;  // 현재 날 수

	// 경과 시간 가져오기
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	accumulatedTime += fTimeElapsed;  // 누적 시간 업데이트

	// 누적 시간이 초 단위 이상인 경우 분과 초 계산
	int totalSeconds = static_cast<int>(accumulatedTime);  // 누적 시간을 초 단위로 변환
	curMinute = totalSeconds / 60;
	curSecond = totalSeconds % 60;


	// 전체 시간 (낮 + 밤)을 기준으로 하루 계산
	float fullCycleTime = serverDayTime + serverNightTime;

	// 서버에서 낮과 밤 전환 여부에 따른 시간 조정
	if (DayTime && !Night) {
		if (accumulatedTime >= serverDayTime) {
			curDay++;
			accumulatedTime -= serverDayTime;  // 낮 시간 초기화 (밤으로 전환)

		}
	}
	else if (Night) {
		if (accumulatedTime >= serverNightTime) {

			accumulatedTime -= serverNightTime;  // 밤 시간 초기화 (낮으로 전환)

		}
	}

	// 업데이트된 시간 계산
	curMinute = static_cast<int>(accumulatedTime) / 60;
	curSecond = static_cast<int>(accumulatedTime) % 60;

}


void CGameFramework::FrameAdvance()
{
	// Scene Change 변경
	try {
		if (isSceneChange && SceneNum < 3)
		{
			if (sceneManager.GetCurrentScene() == SCENEKIND::LOGIN)
			{

				gNetwork.SendLoginfo();

				while (cl_id == -1)
					this_thread::yield();

				ChangeScene(SCENEKIND::LOBBY);
				g_sendqueue.push(SENDTYPE::CHANGE_STAGE);
			}
			else if (sceneManager.GetCurrentScene() == SCENEKIND::LOBBY ||
				sceneManager.GetCurrentScene() == SCENEKIND::FIRE ||
				sceneManager.GetCurrentScene() == SCENEKIND::ICE ||
				sceneManager.GetCurrentScene() == SCENEKIND::NATURE)
			{
				ChangeScene(SCENEKIND::SPACESHIP);
				//
				g_sendqueue.push(SENDTYPE::CHANGE_STAGE);

				if (gNetwork.ClientState == false) // 처음 로비에서 -> 인게임으로 들어가는 상태, 
				{
					g_sendqueue.push(SENDTYPE::CHANGE_SCENE_INGAME_START);

				}
			}
		}
		else if (isSceneChangetoFire) {
			ChangeScene(SCENEKIND::FIRE);
			//
			g_sendqueue.push(SENDTYPE::CHANGE_STAGE);
		}
		else if (isSceneChangetoIce) {
			ChangeScene(SCENEKIND::ICE);
			//
			g_sendqueue.push(SENDTYPE::CHANGE_STAGE);
		}
		else if (isSceneChangetoNature) {
			ChangeScene(SCENEKIND::NATURE);
			//
			g_sendqueue.push(SENDTYPE::CHANGE_STAGE);
		}
		else if (isWin) {
			ChangeScene(SCENEKIND::VICTORY);
		}
		else if (isLose) {
			ChangeScene(SCENEKIND::DEFEAT);
		}

		m_GameTimer.Tick(60.0f);
		ProcessInput();

		AnimateObjects();
		HRESULT hResult = m_pd3dCommandAllocator->Reset();
		hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

		D3D12_RESOURCE_BARRIER d3dResourceBarrier;
		::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
		d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3dResourceBarrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
		d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

		D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * ::gnRtvDescriptorIncrementSize);

		m_pScene->OnPrepareRender(m_pd3dCommandList, m_pCamera, false);


		D3D12_VIEWPORT viewport = m_ShadowMap->Viewport();
		m_pd3dCommandList->RSSetViewports(1, &viewport);
		auto scissorRect = m_ShadowMap->ScissorRect();
		m_pd3dCommandList->RSSetScissorRects(1, &scissorRect);

		::SynchronizeResourceTransition(m_pd3dCommandList, m_ShadowMap->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		m_pd3dCommandList->ClearDepthStencilView(m_ShadowMap->Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		auto dsv = m_ShadowMap->Dsv();
		m_pd3dCommandList->OMSetRenderTargets(0, nullptr, false, &dsv);
		if (m_ShadowMap->GetPipelineState())m_pd3dCommandList->SetPipelineState(m_ShadowMap->GetPipelineState());

		XMFLOAT3 pos;
		XMFLOAT3 dir = XMFLOAT3(-0.3f, -0.85f, -0.3f);
		float radius = 1000;

		XMFLOAT3 targetpos = m_pPlayer->GetPosition();
		XMVECTOR lightDir = XMLoadFloat3(&dir);
		XMVECTOR targetPos = XMLoadFloat3(&targetpos);
		XMVECTOR lightPos = targetPos - 2.0f * radius * lightDir;
		XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

		XMStoreFloat3(&pos, lightPos);

		// Transform bounding sphere to light space.
		XMFLOAT3 sphereCenterLS;
		XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));
		float l = sphereCenterLS.x - radius;
		float b = sphereCenterLS.y - radius;
		float n = sphereCenterLS.z - radius;
		float r = sphereCenterLS.x + radius;
		float t = sphereCenterLS.y + radius;
		float f = sphereCenterLS.z + radius;

		XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
		XMMATRIX T(0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);
		XMMATRIX S = lightView * lightProj * T;

		XMFLOAT4X4 proj;
		XMStoreFloat4x4(&proj, lightProj);

		XMFLOAT4X4 view;
		XMStoreFloat4x4(&view, lightView);

		XMStoreFloat4x4(&m_pShadowMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&view)));
		XMStoreFloat4x4(&m_pShadowMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&proj)));
		XMStoreFloat4x4(&m_pShadowMappedCamera->m_xm4x4ShadowTransform, XMMatrixTranspose(S));
		XMStoreFloat4x4(&m_pCamera->GetCameraInfo()->m_xm4x4ShadowTransform, XMMatrixTranspose(S));

		::memcpy(&m_pShadowMappedCamera->m_xmf3Position, &pos, sizeof(XMFLOAT3));

		D3D12_GPU_VIRTUAL_ADDRESS d3dGPUVirtualAddress = m_pShadowCamera->GetGPUVirtualAddress();
		m_pd3dCommandList->SetDescriptorHeaps(1, &m_pScene->m_pd3dCbvSrvDescriptorHeap);
		m_pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGPUVirtualAddress);


		if (m_pScene) {
			m_pScene->Render(m_pd3dCommandList, m_pCamera, false);			
		}
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
		::SynchronizeResourceTransition(m_pd3dCommandList, m_ShadowMap->Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);


		if (sceneManager.GetCurrentScene() != SCENEKIND::LOGIN && sceneManager.GetCurrentScene() != SCENEKIND::LOBBY)
			UpdateTime();
		UpdateShaderVariables();
		m_pScene->OnPrepareRender(m_pd3dCommandList, m_pCamera);


		// 사용
		D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

		m_pPostProcessingShader->OnPrepareRenderTarget(m_pd3dCommandList, 1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], d3dDsvCPUDescriptorHandle);

		// imgui
		bool isActive = false;

		//if (ImGui::Begin("chat", &isActive))
		//{
		//	static char serverAddress[256] = "";

		//	
		//	ImGui::InputText("Server Address", serverAddress, sizeof(serverAddress));

		//	if (ImGui::Button("Connect"))
		//	{
		//		// Connect to server logic can be added here
		//		std::cout << "Connecting to server at: " << serverAddress << std::endl;
		//		// Example: gNetwork.Connect(serverAddress);
		//	}

		//	ImGui::End();
		//}
		//ImGui::Render();
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pd3dCommandList);


		m_pScene->Render(m_pd3dCommandList, m_pCamera);
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

		//if (m_bRenderBoundingBox) m_pScene->RenderBoundingBox(m_pd3dCommandList, m_pCamera);

		m_pPostProcessingShader->OnPostRenderTarget(m_pd3dCommandList);

		//d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], TRUE, &d3dDsvCPUDescriptorHandle);

		//d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		m_pd3dCommandList->SetDescriptorHeaps(1, &m_pPostProcessingShader->m_pd3dCbvSrvDescriptorHeap);
		//d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_pd3dCommandList->SetGraphicsRootDescriptorTable(12, m_ShadowMap->Srv());
		m_pPostProcessingShader->Render(m_pd3dCommandList, m_pCamera, &m_nDrawOption);

		//m_pScene->Render(m_pd3dCommandList, m_pCamera);

		if (isBlurRender)
		{
			// Render Target Resource Copy to BlurBuffer(For Blur Process)
			::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
			::SynchronizeResourceTransition(m_pd3dCommandList, m_pBlurBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

			m_pd3dCommandList->CopyResource(m_pBlurBuffer, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex]);

			::SynchronizeResourceTransition(m_pd3dCommandList, m_pBlurBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
			::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

			m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
			m_pd3dCommandList->ClearRenderTargetView(m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], Colors::Red, 0, NULL);
			m_pd3dCommandList->OMSetRenderTargets(1, &m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], TRUE, &d3dDsvCPUDescriptorHandle);
			m_pd3dCommandList->SetDescriptorHeaps(1, &m_BlurShader->m_pd3dCbvSrvDescriptorHeap);
			m_BlurShader->Render(m_pd3dCommandList, m_pCamera);
		}
		::SynchronizeResourceTransition(m_pd3dCommandList, m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		hResult = m_pd3dCommandList->Close();

		ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
		m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

		WaitForGpuComplete();
#ifdef _FULLSCREEN

		if (m_pUILayer)
			UILayer::GetInstance()->Render(m_nSwapChainBufferIndex, sceneManager.GetCurrentScene(), isready, curDay, curMinute, curSecond);

#endif // _FULLSCREEN

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
	catch (exception ex) {
		cout << "Err: " << ex.what() << endl;
	}
}

