#pragma once

// Key Code
#define KEY_W 0x57
#define KEY_S 0x53
#define KEY_A 0x41
#define KEY_D 0x44

#ifdef _FULLSCREEN
#define FRAME_BUFFER_WIDTH				1920
#define FRAME_BUFFER_HEIGHT				1080
#else
#define FRAME_BUFFER_WIDTH				640
#define FRAME_BUFFER_HEIGHT				480
#endif

#define DRAW_SCENE_COLOR				'S'

#define DRAW_SCENE_TEXTURE				'T'
#define DRAW_SCENE_LIGHTING				'L'
#define DRAW_SCENE_NORMAL				'N'
#define DRAW_SCENE_Z_DEPTH				'Z'
#define DRAW_SCENE_DEPTH				'D'


#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "Network.h"

extern Network							gNetwork;


struct TIME
{
	float fCurrentMin;
	float fCurrentSec;
};

class UILayer;

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateSwapChainRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

   // void BuildObjects();
	void BuildObjects(int nScene);
	void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void CreateShaderVariables();


	void UpdateTime();

	void UpdateShaderVariables();

	void readyUI();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


	// 서버로부터 받은 좌표 렌더링
public:
	unique_ptr<CBlurShader> m_BlurShader = NULL;
	ID3D12Resource* m_pBlurBuffer = NULL;
	bool isBlurRender = true;

	CPlayer* m_pPlayer = NULL;
	int cl_id;
	void myFunc_SetPosition(int n, int id, XMFLOAT3 position);
	void myFunc_SetMonPosition(int n, XMFLOAT3 position);
	// **함수명 LookRightUp으로 바꿔야 될 듯
	void myFunc_SetLookRightUp(int n, int id, XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right);
	void myFunc_SetMonLookRightUp(int n, XMFLOAT3 Look, XMFLOAT3 Up, XMFLOAT3 Right);

	// 다른 클라들 애니메이션 변경해주는 함수
	void myFunc_SetAnimation(int n, int id, int prevAni, int curAni);
	// 다른 클라들 공격 설정해주는 함수
	void myFunc_SetAttack(int n, int id, bool isAttack);

	void myFunc_SetBlind(int n, int id, bool _isblind);

	CScene* m_pScene = NULL;

	int SceneNum = 0;
	bool isready = false;

	// 시계
	int total = 0;
	int curDay = 0;
	int curMinute = 0;
	int curSecond = 0;
	
	bool DayTime = false;
	bool Night = false;

	ID3D12Resource* m_pd3dcbTime = NULL;
	TIME* m_pTime = NULL;

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBuffers];

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
#endif
	bool						m_bRenderBoundingBox = false;

	CGameTimer					m_GameTimer;



	CCamera						*m_pCamera = NULL;

	UILayer* m_pUILayer1 = NULL;
	UILayer* m_pUILayer2 = NULL;
	UILayer* m_pUILayer = NULL;


	CPostProcessingShader* m_pPostProcessingShader = NULL;

	int								m_nDrawOption = DRAW_SCENE_COLOR;

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[70];
};

