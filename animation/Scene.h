//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"
#include "Network.h"

#define MAX_LIGHTS						16 

#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT				3

extern Network							gNetwork;

struct LIGHT
{
	XMFLOAT4							m_xmf4Ambient;
	XMFLOAT4							m_xmf4Diffuse;
	XMFLOAT4							m_xmf4Specular;
	XMFLOAT3							m_xmf3Position;
	float 								m_fFalloff;
	XMFLOAT3							m_xmf3Direction;
	float 								m_fTheta; //cos(m_fTheta)
	XMFLOAT3							m_xmf3Attenuation;
	float								m_fPhi; //cos(m_fPhi)
	bool								m_bEnable;
	int									m_nType;
	float								m_fRange;
	float								padding;
};										
										
struct LIGHTS							
{										
	LIGHT								m_pLights[MAX_LIGHTS];
	XMFLOAT4							m_xmf4GlobalAmbient;
	int									m_nLights;
};

struct PS_CB_Blend_Factor
{
	XMINT4							m_xmn4BlendFactor;
};

struct SoundData {
	WAVEFORMATEX wfx;
	XAUDIO2_BUFFER buffer;
	BYTE* pData;
};

class CScene
{
public:
    CScene();
    ~CScene();


	//// bgm
	//void PlayBGM(const wchar_t* soundFile) { PlaySound(soundFile, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); };
	//void StopBGM() { PlaySound(NULL, 0, 0); };

	// 충돌체크 함수
	void HandleCollisionEnd(CGameObject* pObject) {}
	virtual bool CheckObjectByObjectCollisions() { return true; }

	//void SendDeadToServer();

	// 바운딩박스
	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {};

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	virtual ID3D12RootSignature* CreateComputeRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetComputeRootSignature() { return(m_pd3dComputeRootSignature); }

	ID3D12RootSignature* CreateRootSignature(ID3D12Device* pd3dDevice, D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags, UINT nRootParameters, D3D12_ROOT_PARAMETER* pd3dRootParameters, UINT nStaticSamplerDescs, D3D12_STATIC_SAMPLER_DESC* pd3dStaticSamplerDescs);

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed);

	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool bIsAnimate = true);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL, bool bIsAnimate = true, int pipelinestate=0);

	ID3D12RootSignature* GetRootSignature() { return m_pd3dGraphicsRootSignature; }

	void ReleaseUploadBuffers();

	ID3D12DescriptorHeap* GetDescriptorHeap() { return m_pd3dCbvSrvDescriptorHeap; }
	static ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap;
	CPlayer* m_pPlayer = NULL;
protected:
	ID3D12RootSignature					*m_pd3dGraphicsRootSignature = NULL;
	ID3D12RootSignature* m_pd3dComputeRootSignature = NULL;


	CBoundingBoxShader* m_pBoundingBoxShader = NULL;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;

public:

	static void CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);

	static void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);


	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, int nResources, ID3D12Resource** ppd3dResources, DXGI_FORMAT* pdxgiSrvFormats);



	float								m_fElapsedTime = 0.0f;

	int									m_nGameObjects = 0;
	CGameObject							**m_ppGameObjects = NULL;

	int									m_nHierarchicalGameObjects = 0;
	CGameObject							**m_ppHierarchicalGameObjects = NULL;


	XMFLOAT3							m_xmf3RotatePosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int									m_nShaders = 0;
	CShader								**m_ppShaders = NULL;

	CSkyBox								*m_pSkyBox = NULL;
	CHeightMapTerrain					*m_pTerrain = NULL;

	LIGHT								*m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource						*m_pd3dcbLights = NULL;
	LIGHTS								*m_pcbMappedLights = NULL;

	virtual string getname() { return m_username; }
	virtual string getpassword() { return m_password; }
	bool m_isUsernameInput = true;  // 입력 모드 (true: 사용자 이름 입력, false: 비밀번호 입력)

	 std::string m_username = "";
	 std::string m_password="";
};

class CLoginScene : public CScene
{
public:
	CLoginScene() { }
	~CLoginScene() { }


	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void HandleSpecialKeyInput(WPARAM wParam);
	void ProcessLogin();
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void ReleaseUploadBuffers();
	void ReleaseObjects();
	
};

class CLobbyScene : public CScene
{
public:
	CLobbyScene() { }
	~CLobbyScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool CheckObjectByObjectCollisions();
	void ReleaseUploadBuffers();
	void ReleaseObjects();
};

class CSpaceShipScene : public CScene
{
public:
	CSpaceShipScene() { }
	~CSpaceShipScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void HandleCollisionEnd(CGameObject* pObject);
	bool CheckObjectByObjectCollisions();
	void ReleaseUploadBuffers();
	void ReleaseObjects();

public:
	BoundingSphere SpaceshipBS;
};

class CIceScene : public CScene
{
public:
	CIceScene() { }
	~CIceScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool CheckObjectByObjectCollisions();
	void ReleaseUploadBuffers();
	void ReleaseObjects();
};

class CFireScene : public CScene
{
public:
	CFireScene() { }
	~CFireScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool CheckObjectByObjectCollisions();
	void ReleaseUploadBuffers();
	void ReleaseObjects();
};

class CGrassScene : public CScene
{
public:
	CGrassScene() { }
	~CGrassScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool CheckObjectByObjectCollisions();
	void ReleaseUploadBuffers();
	void ReleaseObjects();
};
class CWInScene : public CScene
{
public:
	CWInScene() { }
	~CWInScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseUploadBuffers();
	void ReleaseObjects();
};
class CLoseScene : public CScene
{
public:
	CLoseScene() { }
	~CLoseScene() { }

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseUploadBuffers();
	void ReleaseObjects();
};
