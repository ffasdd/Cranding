//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

ID3D12DescriptorHeap *CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

CDescriptorHeap* CScene::m_pDescriptorHeap = NULL;

CDescriptorHeap::CDescriptorHeap()
{
	m_d3dSrvCPUDescriptorStartHandle.ptr = NULL;
	m_d3dSrvGPUDescriptorStartHandle.ptr = NULL;
}

CDescriptorHeap::~CDescriptorHeap()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 1;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	// m_pLights[0].m_bEnable = true;
	// m_pLights[0].m_nType = POINT_LIGHT;
	// m_pLights[0].m_fRange = 300.0f;
	// m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	// m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.3f, 0.8f, 1.0f);
	// m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	// m_pLights[0].m_xmf3Position = XMFLOAT3(230.0f, 330.0f, 480.0f);
	// m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	// m_pLights[1].m_bEnable = true;
	// m_pLights[1].m_nType = SPOT_LIGHT;
	// m_pLights[1].m_fRange = 500.0f;
	// m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	// m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	// m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	// m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	// m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	// m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	// m_pLights[1].m_fFalloff = 8.0f;
	// m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	// m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(-0.3f, -0.85f, -0.3f);
	// m_pLights[3].m_bEnable = true;
	// m_pLights[3].m_nType = SPOT_LIGHT;
	// m_pLights[3].m_fRange = 600.0f;
	// m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	// m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	// m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	// m_pLights[3].m_xmf3Position = XMFLOAT3(550.0f, 330.0f, 530.0f);
	// m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	// m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	// m_pLights[3].m_fFalloff = 8.0f;
	// m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	// m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	// m_pLights[4].m_bEnable = true;
	// m_pLights[4].m_nType = POINT_LIGHT;
	// m_pLights[4].m_fRange = 200.0f;
	// m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	// m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f);
	// m_pLights[4].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	// m_pLights[4].m_xmf3Position = XMFLOAT3(600.0f, 250.0f, 700.0f);
	// m_pLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_pDescriptorHeap = new CDescriptorHeap();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 500); //SuperCobra(17), Gunship(2), Player:Mi24(1), Angrybot()

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature); 

	BuildDefaultLightsAndMaterials();

	XMFLOAT3 xmf3Scale(8.0f, 2.0f, 8.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.3f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/HeightMap.raw"), 257, 257, xmf3Scale, xmf4Color);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 3;
	m_ppHierarchicalGameObjects = new CGameObject*[m_nHierarchicalGameObjects];
	//**
	m_pBoundingBoxShader = new CBoundingBoxShader();
	m_pBoundingBoxShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

	CLoadedModelInfo *pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[0] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(1, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(3, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(4, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(5, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(6, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(7, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(10, 0.5f);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackSpeed(11, 0.5f);

	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	m_ppHierarchicalGameObjects[0]->SetPosition(410.0f, /*m_pTerrain->GetHeight(410.0f, 735.0f)*/0.0f, 735.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(10.0f, 10.0f, 10.0f);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(1, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(3, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(4, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(5, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(10, 0.5f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(11, 0.5f);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, /*m_pTerrain->GetHeight(410.0f, 735.0f)*/0.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(10.0f, 10.0f, 10.0f);

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/mapforzz.bin", NULL);
	m_ppHierarchicalGameObjects[2] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[2]->SetPosition(280.0f, 0.0f, 620.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

//	CLoadedModelInfo *pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut.bin", NULL);
//	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
//	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
//	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f,20.0f, 735.0f);
//	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);
	if (pPlayerModel) delete pPlayerModel;




	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
	if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;

	if (m_ppHierarchicalGameObjects)
	{
		for (int i = 0; i < m_nHierarchicalGameObjects; i++) if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->Release();
		delete[] m_ppHierarchicalGameObjects;
	}

	if (m_pBoundingBoxShader) m_pBoundingBoxShader->Release();

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[10];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 5;
	pd3dDescriptorRanges[8].BaseShaderRegister = 14; //t14: Texture2D<float4>
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 19; //t19: Texture2DArray
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[16];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[9].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[9].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[10].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[10].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[11].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[12].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[12].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[8]; //Texture2D
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[14].Descriptor.ShaderRegister = 9; //DrawOptions
	pd3dRootParameters[14].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[15].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[15].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[9]; //Texture2DArray
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nHierarchicalGameObjects; i++) m_ppHierarchicalGameObjects[i]->ReleaseUploadBuffers();
}

bool CScene::CheckObjectByObjectCollisions(CGameObject* pTargetGameObject)
{
	// 불안 -> 좀 많이 바꿈
	// **충돌체크 m_nHierarchicalGameObjects끼리만 하면 되는거 맞겠지??
	//for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	//{
		for (int j = 0; j < m_nHierarchicalGameObjects; j++)
		{
			//CGameObject* pGameObject = m_ppObjectShaders[i]->m_ppObjects[j];
			if (m_ppHierarchicalGameObjects[j]->m_xmBoundingBox.Intersects(pTargetGameObject->m_xmBoundingBox)) return(true);
		}
	//}
	return(false);
}

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle = m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle = m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle.ptr = m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle.ptr = m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

	m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dCbvCPUDescriptorStartHandle;
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dCbvGPUDescriptorStartHandle;
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dSrvCPUDescriptorStartHandle;
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle = m_pDescriptorHeap->m_d3dSrvGPUDescriptorStartHandle;
}

void CScene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = pd3dConstantBuffer->GetGPUVirtualAddress();
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle;
	m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}


D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle;
	m_pDescriptorHeap->m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dResource, DXGI_FORMAT dxgiSrvFormat)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3dShaderResourceViewDesc.Format = dxgiSrvFormat;
	d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
	d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
	d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle;
	pd3dDevice->CreateShaderResourceView(pd3dResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dSrvGPUDescriptorHandle);
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootParameterIndex(nIndex, nRootParameterStartIndex + nIndex);
	}
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle);
		m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}



void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetResource(i);
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
			m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
			pTexture->SetGpuDescriptorHandle(i, m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle);
			m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int j = 0; j < nRootParameters; j++) pTexture->SetRootParameterIndex(j, nRootParameterStartIndex + j);
}

void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, int nResources, ID3D12Resource** ppd3dResources, DXGI_FORMAT* pdxgiSrvFormats)
{
	for (int i = 0; i < nResources; i++)
	{
		if (ppd3dResources[i])
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
			d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d3dShaderResourceViewDesc.Format = pdxgiSrvFormats[i];
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
			d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
			d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			pd3dDevice->CreateShaderResourceView(ppd3dResources[i], &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle);
			m_pDescriptorHeap->m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
			m_pDescriptorHeap->m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// Object 매니저를 통해서 여기서 관리해줘도 될거같음 
	// 그게더 좋을수도? 

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		//case 'W':
		//	m_pPlayer->Move(DIR_FORWARD, 25.25f, true);
		//	// send 
		//	g_sendqueue.push(SENDTYPE::MOVE);
		//	break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed);
	for (int i = 0; i < m_nHierarchicalGameObjects; i++) if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->Animate(fTimeElapsed);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}


	//static float fAngle = 0.0f;
	//fAngle += 1.50f;
	//XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Rotate(0.0f, -fAngle, 0.0f);
	//XMFLOAT3 xmf3Position = Vector3::TransformCoord(XMFLOAT3(65.0f, 0.0f, 0.0f), xmf4x4Rotate);

}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_LIGHT, d3dcbLightsGpuVirtualAddress); //Lights

	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	//pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_MATERIAL, d3dcbMaterialsGpuVirtualAddress); //Materials

}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	/*if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);*/

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i])
		{
			m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}	
	
}

//**
void CScene::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_pBoundingBoxShader->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->RenderBoundingBox(pd3dCommandList, pCamera);
	}
	m_pPlayer->RenderBoundingBox(pd3dCommandList, pCamera);
}

