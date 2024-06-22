//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
extern concurrency::concurrent_queue<SENDTYPE> g_sendqueue;
extern Network							gNetwork;
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


bool CScene::CheckObjectByObjectCollisions(CGameObject* pTargetGameObject)
{
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		// �ʰ� �浹�� ���
		if (i == 0) 
		{
			CGameObject* pMapObject = m_ppHierarchicalGameObjects[0]->m_pChild->m_pChild;

			for (int j = 0; j < m_ppHierarchicalGameObjects[0]->m_pChild->nChilds; j++)
			{
				const char* str = pMapObject->m_pstrFrameName;

				if (pMapObject->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
				{
					// grassmap ���� �ٸ��� �浹
					if (!strcmp(str, "bbgrassmap"))
					{
						m_pPlayer->isGrassMap = true;
						return(true);
					}

					// firemap ���� �ٸ��� �浹
					else if (!strcmp(str, "bbfiremap"))
					{
						m_pPlayer->isFireMap = true;
						return(true);
					}

					// icemap ���� �ٸ��� �浹
					else if (!strcmp(str, "bbicemap"))
					{
						m_pPlayer->isIceMap = true;
						return(true);
					}

					// �ٴ��� �浹 üũ �ȵǵ���
					/*else */if (!strcmp(str, "Plane"))
						pMapObject = pMapObject->m_pSibling;

					else
						return(true);
				}
					pMapObject = pMapObject->m_pSibling;

					if (pMapObject == NULL)break;
			}
		}
	
		//else if( i == 1 || i == 2) 
		//{
		//	/*if (m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
		//	{
		//		return(true);
		//	}*/
		//	return false;
		//}
	
		else if (i > 2)
		{
	
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsAttack == true && m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
	
				gNetwork.SendAttackCollision(g_monsters[i - 3].getId());
	
				return(true);
			}
		}
	}
	return(false);
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

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	//if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
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
			m_ppShaders[i] = nullptr;
		}
		delete[] m_ppShaders;
		m_ppShaders = nullptr;
	}

	if (m_pSkyBox)
	{
		delete m_pSkyBox;
		m_pSkyBox = nullptr;
	}

	if (m_ppHierarchicalGameObjects)
	{
		for (int i = 0; i < m_nHierarchicalGameObjects; i++) if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->Release();
		delete[] m_ppHierarchicalGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) {
		delete[] m_pLights;
		m_pLights = nullptr;
	}
	
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
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera (b1)
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject (b2)
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights (b4)
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

ID3D12RootSignature* CScene::CreateComputeRootSignature(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 0; //t0: Texture2D
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //u0: RWTexture2D
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[2];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[0].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Texture2D
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //RWTexture2D
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	ID3D12RootSignature* pd3dComputeRootSignature = CreateRootSignature(pd3dDevice, d3dRootSignatureFlags, _countof(pd3dRootParameters), pd3dRootParameters, 0, NULL);

	return(pd3dComputeRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
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

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	//for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nHierarchicalGameObjects; i++) m_ppHierarchicalGameObjects[i]->ReleaseUploadBuffers();
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

void CScene::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	m_pBoundingBoxShader->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->RenderBoundingBox(pd3dCommandList, pCamera);
	}

	m_pPlayer->RenderBoundingBox(pd3dCommandList, pCamera);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// Object �Ŵ����� ���ؼ� ���⼭ �������൵ �ɰŰ��� 
	// �װԴ� ��������? 

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

ID3D12RootSignature* CScene::CreateRootSignature(ID3D12Device* pd3dDevice, D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags, UINT nRootParameters, D3D12_ROOT_PARAMETER* pd3dRootParameters, UINT nStaticSamplerDescs, D3D12_STATIC_SAMPLER_DESC* pd3dStaticSamplerDescs)
{
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = nRootParameters;
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = nStaticSamplerDescs;
	d3dRootSignatureDesc.pStaticSamplers = pd3dStaticSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3D12RootSignature* pd3dRootSignature = NULL;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dRootSignature);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed);

	//if (m_nHierarchicalGameObjects > 3)
	//{
	//	for (int i = 0; i < 3; i++) if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->Animate(fTimeElapsed);
	//}
	//else
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
	//if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i] && m_ppHierarchicalGameObjects[i]->isdraw == true)
		{
			m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}	
	
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// �α��ξ�
//////////////////////////////////////////////////////////////////////////////////////////
//
// �α��ξ�
void CLoginScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 4;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/plane.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(400.0f, -20.0f, 400.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(8.0f, 8.0f, 8.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/dance.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 2);
	m_ppHierarchicalGameObjects[1]->SetPosition(30.0f, 0.0f, -65.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[1]->Rotate(-20.0f, 170.0f, 00.0f);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.7);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->SetPosition(50.0f, 0.0f, -85.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[2]->Rotate(-20.0f, 170.0f, 00.0f);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.6);

	m_ppHierarchicalGameObjects[3] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[3]->SetPosition(10.0f, 0.0f, -85.0f);
	m_ppHierarchicalGameObjects[3]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[3]->Rotate(-20.0f, 170.0f, 0.0f);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.6);

	if (pPlayerModel) delete pPlayerModel;

}

void CLoginScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CLoginScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
void CLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 3;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/robbymap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 0.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 11);

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

	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 11);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;
}

void CLobbyScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CLobbyScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
void CSpaceShipScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 3 + 10;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/spaceshipmap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, -50.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 11);

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

	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(1, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(3, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(4, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(5, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5);
	//m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(10, 0.5);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 11);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(1, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(2, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(3, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(4, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(5, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(6, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(7, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(8, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(9, 0.5);
	//m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(10, 0.5);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;


	//// ���⼭���� �� ��ü �׸���
	//CLoadedModelInfo* pIceEnemyModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);

	//// �� 10����
	//for (int i = 0; i < 10; i++)
	//{
	//	m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel, 7);

	//	// 0 : idle, 1 : �ȱ�, 2 : �޸���, 3 : ����, 4 : ����, 5 : �޼� ����, 6 : ������ ����
	//	// animation Track -> ���� ���� ���� �ִϸ��̼� ���� / animation Set -> ���� bin ���� �� �ִϸ��̼� ����(����, ��ȣ)
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	//	m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	//	m_ppHierarchicalGameObjects[3 + i]->SetScale(20.0f, 20.0f, 20.0f);
	//}
	CLoadedModelInfo* pIceEnemyModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[3] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel1, 7);

	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[3]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[4] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel2, 7);

	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[4]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[4]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel3 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[5] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel3, 7);

	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[5]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[5]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel4 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[6] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel4, 7);

	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[6]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[6]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel5 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[7] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel5, 7);

	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[7]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[7]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel6 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[8] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel6, 7);

	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[8]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[8]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel7 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[9] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel7, 7);

	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[9]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[9]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel8 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[10] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel8, 7);

	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[10]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[10]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel9 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[11] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel9, 7);

	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[11]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[11]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pIceEnemyModel10 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	m_ppHierarchicalGameObjects[12] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel10, 7);

	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[12]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[12]->SetScale(20.0f, 20.0f, 20.0f);

	if (pIceEnemyModel1) delete pIceEnemyModel1;
	if (pIceEnemyModel2) delete pIceEnemyModel2;
	if (pIceEnemyModel3) delete pIceEnemyModel3;
	if (pIceEnemyModel4) delete pIceEnemyModel4;
	if (pIceEnemyModel5) delete pIceEnemyModel5;
	if (pIceEnemyModel6) delete pIceEnemyModel6;
	if (pIceEnemyModel7) delete pIceEnemyModel7;
	if (pIceEnemyModel8) delete pIceEnemyModel8;
	if (pIceEnemyModel9) delete pIceEnemyModel9;
	if (pIceEnemyModel10) delete pIceEnemyModel10;
}

void CSpaceShipScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CSpaceShipScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

//////////////////////////////////////////////////////////////////////////////////////////
//

void CIceScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 3 + 10;

	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/icemap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 0.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_Sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

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

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel) delete pPlayerModel;

	// ���⼭���� �� ��ü �׸���
	CLoadedModelInfo* pIceEnemyModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);

	for (int i = 0; i < 10; i++)
	{
		m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel, 7);

		// 0 : idle, 1 : �ȱ�, 2 : �޸���, 3 : ����, 4 : ����, 5 : �޼� ����, 6 : ������ ����
		// animation Track -> ���� ���� ���� �ִϸ��̼� ���� / animation Set -> ���� bin ���� �� �ִϸ��̼� ����(����, ��ȣ)
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

		m_ppHierarchicalGameObjects[3 + i]->isdraw = false;

		float posX = (rand() % 2000) / 10.0;
		float posY = (rand() % 2000) / 10.0;

		m_ppHierarchicalGameObjects[3 + i]->SetPosition(posX, 0.0f, posY);
		m_ppHierarchicalGameObjects[3 + i]->SetScale(10.0f, 10.0f, 10.0f);
	}
}

void CIceScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CIceScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

void CFireScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 3 + 10;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/firemap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 00.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_Sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

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

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel) delete pPlayerModel;

	// ���⼭���� �� ��ü �׸���
	CLoadedModelInfo* pIceEnemyModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);

	for (int i = 0; i < 10; i++)
	{
		m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel, 7);

		// 0 : idle, 1 : �ȱ�, 2 : �޸���, 3 : ����, 4 : ����, 5 : �޼� ����, 6 : ������ ����
		// animation Track -> ���� ���� ���� �ִϸ��̼� ���� / animation Set -> ���� bin ���� �� �ִϸ��̼� ����(����, ��ȣ)
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

		m_ppHierarchicalGameObjects[3 + i]->isdraw = false;

		float posX = (rand() % 2000) / 10.0;
		float posY = (rand() % 2000) / 10.0;
		m_ppHierarchicalGameObjects[3 + i]->SetPosition(posX, 0.0f, posY);
		m_ppHierarchicalGameObjects[3 + i]->SetScale(10.0f, 10.0f, 10.0f);
	}
}
void CFireScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CFireScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

void CGrassScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 3 + 10;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/grassmap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 00.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_Sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

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

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 11);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(8, 8);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(9, 9);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(10, 10);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(8, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(9, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(10, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel) delete pPlayerModel;

	// ���⼭���� �� ��ü �׸���
	CLoadedModelInfo* pIceEnemyModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);

	for (int i = 0; i < 10; i++)
	{

		m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModel, 7);

		// 0 : idle, 1 : �ȱ�, 2 : �޸���, 3 : ����, 4 : ����, 5 : �޼� ����, 6 : ������ ����
		// animation Track -> ���� ���� ���� �ִϸ��̼� ���� / animation Set -> ���� bin ���� �� �ִϸ��̼� ����(����, ��ȣ)
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
		m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

		m_ppHierarchicalGameObjects[3 + i]->isdraw = false;

		float posX = (rand() % 2000) / 10.0;
		float posY = (rand() % 2000) / 10.0;
		m_ppHierarchicalGameObjects[3 + i]->SetPosition(posX, 0.0f, posY);
		m_ppHierarchicalGameObjects[3 + i]->SetScale(10.0f, 10.0f, 10.0f);
	}
	if (pIceEnemyModel) delete pIceEnemyModel;
}
void CGrassScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CGrassScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}