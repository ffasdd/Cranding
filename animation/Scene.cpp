//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "xaudio2.lib")

extern Network							gNetwork;
ID3D12DescriptorHeap* CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;

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

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//m_pDescriptorHeap = new CDescriptorHeap();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 500); //SuperCobra(17), Gunship(2), Player:Mi24(1), Angrybot()

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	//if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
	//if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
		m_ppGameObjects = nullptr;
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
		for (int i = 0; i < m_nHierarchicalGameObjects; i++)
			if (m_ppHierarchicalGameObjects[i]) {
				m_ppHierarchicalGameObjects[i]->Release();
				cout << "=========================================" << endl;
			}
		delete[] m_ppHierarchicalGameObjects;
		m_ppHierarchicalGameObjects = nullptr;
	}

	ReleaseShaderVariables();

	if (m_pLights) {
		delete[] m_pLights;
		m_pLights = nullptr;
	}
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

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
	pd3dDescriptorRanges[5].BaseShaderRegister = 13; //t13: gtxtSkyBoxTextureDay
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 2; //t2: shadow
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

	D3D12_ROOT_PARAMETER pd3dRootParameters[17];

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

	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[16].Descriptor.ShaderRegister = 10; //blendFactor
	pd3dRootParameters[16].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];

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

	pd3dSamplerDescs[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].MipLODBias = 0;
	pd3dSamplerDescs[2].MaxAnisotropy = 16;
	pd3dSamplerDescs[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = 0;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
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

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
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

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);
	if (SUCCEEDED(hResult))
	{
		m_pd3dCbvSrvDescriptorHeap->SetName(L"CScene::CreateCbvSrvDescriptorHeaps");
	}
	else
	{
		// Print the HRESULT error code
		std::wcout << L"Failed to create descriptor heap. HRESULT = " << hResult << std::endl;

		// Check if the error is related to the device being removed
		HRESULT deviceRemovedReason = pd3dDevice->GetDeviceRemovedReason();
		if (deviceRemovedReason != DXGI_ERROR_DEVICE_REMOVED)
		{
			// Print device removed reason if available
			std::wcout << L"Device removed reason HRESULT = " << deviceRemovedReason << std::endl;
		}
		else
		{
			// Handle the case where the device has been removed
			std::wcout << L"Device has been removed or lost." << std::endl;

			// You might also want to include additional diagnostic information
			// depending on the specific requirements of your application.
		}
	}


	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);

}

void CScene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}




void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetResource(i);
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
			pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
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
			pd3dDevice->CreateShaderResourceView(ppd3dResources[i], &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

//void CScene::SendDeadToServer()
//{
//}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	/*for (int i = 2; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i]->GetHealth() <= 0)
			m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = true;
	}*/

	if (g_clients[gNetwork.my_id].getHp() < 0 && m_pPlayer->m_pSkinnedAnimationController->m_bIsDead == false) {
		g_clients[gNetwork.my_id].setHp(0);
		gNetwork.SendPlayerDead();
		//m_pPlayer->SetHealth(0);
		m_pPlayer->m_pSkinnedAnimationController->m_bIsDead = true;
		

	}
	// monster dead
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		//if (m_ppHierarchicalGameObjects[i]) m_ppHierarchicalGameObjects[i]->Animate(fTimeElapsed);
		if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController)
		{
			if (i > 2 && m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead)
			{
				m_ppHierarchicalGameObjects[i]->m_bHasCollided = false;

				switch (m_ppHierarchicalGameObjects[i]->GetMonsType())
				{
				case MONSTERTYPE::NIGHT:
				{
					cout << "Night" << endl;

					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					//m_ppHierarchicalGameObjects[i]->isdraw = false;
					gNetwork.SendAttackCollision(g_monsters[i - 3].getId(), MonsterType::Night);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
				}
				break;
				case MONSTERTYPE::ICE:
				{
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					gNetwork.SendAttackCollision(g_ice_monsters[i - 3].getId(), MonsterType::Ice);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
				}
				break;
				case MONSTERTYPE::FIRE:
				{
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					gNetwork.SendAttackCollision(g_fire_monsters[i - 3].getId(), MonsterType::Fire);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
				}
				break;
				case MONSTERTYPE::NATURE:
				{
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					gNetwork.SendAttackCollision(g_nature_monsters[i - 3].getId(), MonsterType::Nature);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
				}
				break;
				case MONSTERTYPE::ICEBOSS:
				{
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					gNetwork.SendAttackCollision(0, MonsterType::Ice_Boss);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
					m_ppHierarchicalGameObjects[14]->isdraw = true;
				}
				break;
				case MONSTERTYPE::FIREBOSS:
				{
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					m_ppHierarchicalGameObjects[i]->isdraw = false;
					m_ppHierarchicalGameObjects[14]->isdraw = true;
					gNetwork.SendAttackCollision(0, MonsterType::Fire_Boss);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
				}
				break;
				case MONSTERTYPE::NATUREBOSS:
				{
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsDead = false;
					gNetwork.SendAttackCollision(0, MonsterType::Nature_Boss);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
					m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, true);
					m_ppHierarchicalGameObjects[14]->isdraw = true;
				}
				break;

				default:
					break;
				}
			}
		}
	}

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}


	// 
	//static float fAngle = 0.0f;
	//fAngle += 1.50f;
	//XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Rotate(0.0f, -fAngle, 0.0f);
	//XMFLOAT3 xmf3Position = Vector3::TransformCoord(XMFLOAT3(65.0f, 0.0f, 0.0f), xmf4x4Rotate);

}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool bIsAnimate)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	if (bIsAnimate) pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, bool bIsAnimate, int pipelinestate)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Light

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	//if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i] && m_ppHierarchicalGameObjects[i]->isdraw == true)
		{
			//cout << i << endl;
			if (bIsAnimate) m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera, 0, pipelinestate);
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

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/plane.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(400.0f, -30.0f, 400.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(15.0f, 8.0f, 15.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/dance.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);


	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(2, 1.3);

	m_ppHierarchicalGameObjects[1]->SetPosition(30.0f, 0.0f, -65.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[1]->Rotate(-20.0f, 170.0f, 00.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[2]->SetPosition(50.0f, 0.0f, -85.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[2]->Rotate(-20.0f, 170.0f, 00.0f);

	m_ppHierarchicalGameObjects[3] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 5);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);

	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackEnable(4, false);

	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[3]->SetPosition(10.0f, 0.0f, -85.0f);
	m_ppHierarchicalGameObjects[3]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[3]->Rotate(-20.0f, 170.0f, 0.0f);

	if (pPlayerModel) delete pPlayerModel;

}


void  CLoginScene::HandleSpecialKeyInput(WPARAM wParam)
{
	if (wParam == VK_TAB) // Switch input mode when Tab key is pressed
	{
		m_isUsernameInput = !m_isUsernameInput;
	}
}

void CLoginScene::ProcessLogin()
{
	// Implement login processing logic here
	// For example, validate credentials, communicate with a server, etc.
	// For demonstration, we'll just output to console
	printf("Username: %s\n", m_username.c_str());
	printf("Password: %s\n", m_password.c_str());
}

void CLoginScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{

	case WM_KEYDOWN:
		if (m_isUsernameInput)
		{
			// Add character to username
			if (wParam == '\r') // Enter key, switch to password input
			{
				m_isUsernameInput = false;
			}
			else if (wParam == '\b') // Backspace key
			{
				if (!m_username.empty())
					m_username.pop_back();
			}
			else if (wParam >= ' ' && wParam <= '~') // Valid printable characters
			{
				m_username += wParam;
			}
		}
		else
		{
			// Add character to password
			if (wParam == '\r') // Enter key, process login
			{
				ProcessLogin();
			}
			else if (wParam == '\b') // Backspace key
			{
				if (!m_password.empty())
					m_password.pop_back();
			}
			else if (wParam >= ' ' && wParam <= '~') // Valid printable characters
			{
				m_password += wParam;
			}
		}
		HandleSpecialKeyInput(wParam);
		break;
	default:
		break;
	}
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

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/lobbymap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 0.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 8);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 8);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, 20.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;
}

bool CLobbyScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < 1; i++)
	{
		// �ʰ� �浹�� ���
		if (i == 0)
		{
			m_ppHierarchicalGameObjects[i]->m_bWasColliding = m_ppHierarchicalGameObjects[i]->m_bIsColliding;
			m_ppHierarchicalGameObjects[i]->m_bIsColliding = false; // 현재 프레임의 충돌 상태 초기화

			CGameObject* pMapObject = m_ppHierarchicalGameObjects[0]->m_pChild->m_pChild;

			for (int j = 0; j < m_ppHierarchicalGameObjects[0]->m_pChild->nChilds; j++)
			{
				const char* str = pMapObject->m_pstrFrameName;

				if (pMapObject->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
				{
					m_ppHierarchicalGameObjects[i]->m_bIsColliding = true;
					if (!strcmp(str, "bbfire"))
					{
						m_pPlayer->isFireMap = true;
						return true;
					}
					else if (!strcmp(str, "bbice"))
					{
						m_pPlayer->isIceMap = true;
						return true;
					}
					else if (!strcmp(str, "bbgrass"))
					{
						m_pPlayer->isNatureMap = true;
						return true;
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
	}


	//if (m_ppHierarchicalGameObjects[0]->m_bWasColliding && !m_ppHierarchicalGameObjects[0]->m_bIsColliding) {
	//	// 충돌이 종료되었을 때 수행할 작업
	//	HandleCollisionEnd(m_ppHierarchicalGameObjects[0]);
	//}
	return(false);
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

	const int FireMonsterNum = 10;
	const int IceMonsterNum = 10;
	const int GrassMonsterNum = 10;

	cout << "inside builbobj" << endl;
	m_nHierarchicalGameObjects = 3 + FireMonsterNum + IceMonsterNum + GrassMonsterNum;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/spaceshipmap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, -49.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);

	if (map) delete map;


	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 8);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 8);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;


	cout << "make mons" << endl;
	// 각 몬스터 유형에 대한 모델 로드
	CLoadedModelInfo* pFireMonModels[FireMonsterNum];
	CLoadedModelInfo* pIceMonModels[IceMonsterNum];
	CLoadedModelInfo* pGrassMonModels[GrassMonsterNum];

	for (int i = 0; i < FireMonsterNum; ++i) {
		pFireMonModels[i] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	}

	for (int i = 0; i < IceMonsterNum; ++i) {
		pIceMonModels[i] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Icicle.bin", NULL);
	}

	for (int i = 0; i < GrassMonsterNum; ++i) {
		pGrassMonModels[i] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Slime_Cube.bin", NULL);
	}

	// 몬스터 객체 배열 초기화
	const int FireMonsterStartIndex = 3;
	const int IceMonsterStartIndex = FireMonsterStartIndex + FireMonsterNum; // 13
	const int GrassMonsterStartIndex = IceMonsterStartIndex + IceMonsterNum; // 23
	const int TotalMonsters = FireMonsterNum + IceMonsterNum + GrassMonsterNum; // 30

	for (int i = 0; i < TotalMonsters; ++i) {

	
		if (i < FireMonsterNum) {
			m_ppHierarchicalGameObjects[FireMonsterStartIndex + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFireMonModels[i], 6);
			for (int j = 0; j <= 5; ++j) {
				m_ppHierarchicalGameObjects[FireMonsterStartIndex + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(j, j);
				m_ppHierarchicalGameObjects[FireMonsterStartIndex + i]->m_pSkinnedAnimationController->SetTrackEnable(j, !(j == 0 || j == 2 || j == 3 || j == 4 || j == 5));
			}
			m_ppHierarchicalGameObjects[FireMonsterStartIndex + i]->SetScale(20.0f, 20.0f, 20.0f);
			m_ppHierarchicalGameObjects[FireMonsterStartIndex + i]->SetPosition(0.f, -500.0f, -0.f);
			m_ppHierarchicalGameObjects[FireMonsterStartIndex + i]->SetMonsType(MONSTERTYPE::NIGHT);
		}
		else if (i < FireMonsterNum + IceMonsterNum) {
			int iceIndex = i - FireMonsterNum;
			m_ppHierarchicalGameObjects[IceMonsterStartIndex + iceIndex] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceMonModels[iceIndex], 7);
			for (int j = 0; j <= 6; ++j) {
				m_ppHierarchicalGameObjects[IceMonsterStartIndex + iceIndex]->m_pSkinnedAnimationController->SetTrackAnimationSet(j, j);
				m_ppHierarchicalGameObjects[IceMonsterStartIndex + iceIndex]->m_pSkinnedAnimationController->SetTrackEnable(j, !(j == 0 || j == 2 || j == 3 || j == 4 || j == 5 || j == 6));
			}
			m_ppHierarchicalGameObjects[IceMonsterStartIndex + iceIndex]->SetScale(20.0f, 20.0f, 20.0f);
			m_ppHierarchicalGameObjects[IceMonsterStartIndex + iceIndex]->SetPosition(0.f, -500.0f, -0.f);
			m_ppHierarchicalGameObjects[IceMonsterStartIndex + iceIndex]->SetMonsType(MONSTERTYPE::NIGHT);
		}
		else {
			int grassIndex = i - FireMonsterNum - IceMonsterNum;
			m_ppHierarchicalGameObjects[GrassMonsterStartIndex + grassIndex] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pGrassMonModels[grassIndex], 6);
			for (int j = 0; j <= 5; ++j) {
				m_ppHierarchicalGameObjects[GrassMonsterStartIndex + grassIndex]->m_pSkinnedAnimationController->SetTrackAnimationSet(j, j);
				m_ppHierarchicalGameObjects[GrassMonsterStartIndex + grassIndex]->m_pSkinnedAnimationController->SetTrackEnable(j, !(j == 0 || j == 2 || j == 3 || j == 4 || j == 5));
			}
			m_ppHierarchicalGameObjects[GrassMonsterStartIndex + grassIndex]->SetScale(20.0f, 20.0f, 20.0f);
			m_ppHierarchicalGameObjects[GrassMonsterStartIndex + grassIndex]->SetPosition(0.f, -500.0f, -0.f);
			m_ppHierarchicalGameObjects[GrassMonsterStartIndex + grassIndex]->SetMonsType(MONSTERTYPE::NIGHT);
		}

	}

	//PlayBGM(L"Sound/Day.wav");

	// spaceship boundingsphere
	SpaceshipBS.Center = { 250.0f, 10.0f, 750.0f };
	SpaceshipBS.Radius = 50.0f;

	// 메모리 해제
	for (int i = 0; i < FireMonsterNum; ++i) {
		if (pFireMonModels[i]) delete pFireMonModels[i];
	}

	for (int i = 0; i < IceMonsterNum; ++i) {
		if (pIceMonModels[i]) delete pIceMonModels[i];
	}

	for (int i = 0; i < GrassMonsterNum; ++i) {
		if (pGrassMonModels[i]) delete pGrassMonModels[i];
	}
}

void CSpaceShipScene::HandleCollisionEnd(CGameObject* pObject) {
	// 충돌 종료 시 수행할 작업 구현
	std::cout << "Collision ended with object: " << pObject->m_pstrFrameName << std::endl;
	m_pPlayer->isFireMap = false;
	m_pPlayer->isIceMap = false;
	m_pPlayer->isNatureMap = false;

	// 필요에 따라 추가 작업 수행
	pObject->m_bWasColliding = false; // 충돌 종료 후 상태 초기화
}

bool CSpaceShipScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		// 맵 오브젝트와의 충돌 체크
		if (i == 0)
		{
			m_ppHierarchicalGameObjects[i]->m_bWasColliding = m_ppHierarchicalGameObjects[i]->m_bIsColliding;
			m_ppHierarchicalGameObjects[i]->m_bIsColliding = false; // 현재 프레임의 충돌 상태 초기화

			CGameObject* pMapObject = m_ppHierarchicalGameObjects[0]->m_pChild->m_pChild;

			for (int j = 0; j < m_ppHierarchicalGameObjects[0]->m_pChild->nChilds; j++)
			{
				const char* str = pMapObject->m_pstrFrameName;

				if (pMapObject->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
				{
					m_ppHierarchicalGameObjects[i]->m_bIsColliding = true;

					if (!strcmp(str, "bbfire"))
					{
						m_pPlayer->isFireMap = true;
						return true;
					}
					else if (!strcmp(str, "bbice"))
					{
						m_pPlayer->isIceMap = true;
						return true;
					}
					else if (!strcmp(str, "bbgrass"))
					{
						m_pPlayer->isNatureMap = true;
						return true;
					}

					if (!strcmp(str, "Plane"))
						pMapObject = pMapObject->m_pSibling;
					else
						return true;
				}
				pMapObject = pMapObject->m_pSibling;

				if (pMapObject == NULL) break;
			}
		}

		// fire monster와의 충돌 체크
		else if (i > 2 && i < 13)
		{
			// monster with player's sword(attack mode)
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild
					->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;
				//// send  
				gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Night);

				return false;
			}
			// fire monster's hand with player (when firemonster attack)
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& (m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_xmBoundingBox)
					||
					m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				//m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 5);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 5);

				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
				g_clients[gNetwork.my_id].is_damage = true;
				gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);
				//g_sendqueue.push(SENDTYPE::PLAYER_HIT);

				return false;
			}
			// fire monster's hand with spaceship
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& (this->SpaceshipBS.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_xmBoundingBox)
					||
					this->SpaceshipBS.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				gNetwork.SendMonsterHitSpaceship(g_monsters[i - 3].getId());

				return false;
			}
		}
		// ice monster와의 충돌 체크
		else if (i >= 13 && i < 23)
		{
			// monster with player's sword(attack mode)
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;

				gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Night);

				return false;
			}
			// ice monster's hand with player (when ice monster attack)
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& (m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_xmBoundingBox)
					||
					m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 5);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
				g_clients[gNetwork.my_id].is_damage = true;
				gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);

				return false;
			}
			// ice monster's hand with spaceship
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& (this->SpaceshipBS.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_xmBoundingBox)
					||
					this->SpaceshipBS.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;

				return false;
			}
		}
		// grass monster와의 충돌 체크
		else if (i >= 23 && i < 33)
		{
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;

				gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Night);

				return false;
			}
			// grass monster's head with player (when grass monster attack)
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_xmBoundingBox))
			{
				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 5);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
				g_clients[gNetwork.my_id].is_damage = true;
				gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);

				return false;
			}
			// grass monster's head with spaceship
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& this->SpaceshipBS.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_xmBoundingBox))
			{
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;

				return false;
			}
		}
	}

	// 충돌이 종료되었을 때 충돌 상태 초기화
	if (m_ppHierarchicalGameObjects[0]->m_bWasColliding && !m_ppHierarchicalGameObjects[0]->m_bIsColliding)
	{
		HandleCollisionEnd(m_ppHierarchicalGameObjects[0]);
		m_pPlayer->m_bHasCollided = false; // 충돌 상태 초기화
	}

	return false;
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

	m_nHierarchicalGameObjects = 1 + 2 + 10 + 1 + 1; // map(1) + another player(2) + monster(10) + boss(1) + boss item(1)

	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/icemap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 11.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);
	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 8);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(6, 1.5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(7, 1.5);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 8);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(6, 1.5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(7, 1.5);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;

	// 10개의 CLoadedModelInfo 인스턴스를 담을 배열
	CLoadedModelInfo* pIceEnemyModels[10];

	// 10개의 다른 모델을 로드
	for (int i = 0; i < 10; ++i) {
		pIceEnemyModels[i] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Icicle.bin", NULL);
	}

	// 각 모델에 대해 CPlayerObject를 생성
	for (int i = 0; i < 10; i++)
	{
		m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceEnemyModels[i], 7);

		// 애니메이션 트랙 설정 및 일부 트랙 비활성화
		for (int j = 0; j <= 6; ++j) {
			m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(j, j);
			m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(j, !(j == 0 || j == 2 || j == 3 || j == 4 || j == 5 || j == 6)); // 특정 트랙을 제외하고 모두 활성화
		}

		// 초기 속성 설정
		m_ppHierarchicalGameObjects[3 + i]->isdraw = false;

		float posX = float((rand() % 2000) / 10.0);
		float posY = float((rand() % 2000) / 10.0);

		m_ppHierarchicalGameObjects[3 + i]->SetPosition(posX, -200.0f, posY);
		m_ppHierarchicalGameObjects[3 + i]->SetScale(10.0f, 10.0f, 10.0f);
		m_ppHierarchicalGameObjects[3 + i]->SetMonsType(MONSTERTYPE::ICE);
	}

	// 로드된 모든 모델 해제
	for (int i = 0; i < 10; ++i) {
		if (pIceEnemyModels[i]) delete pIceEnemyModels[i];
	}

	CLoadedModelInfo* pIceBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Ice_Mage.bin", NULL);

	m_ppHierarchicalGameObjects[13] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceBossModel, 6);

	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);

	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(5, false);

	m_ppHierarchicalGameObjects[13]->SetPosition(410.0f, -200.0f, 735.0f);
	m_ppHierarchicalGameObjects[13]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[13]->SetMonsType(MONSTERTYPE::ICEBOSS);

	if (pIceBossModel) delete pIceBossModel;

	CLoadedModelInfo* pIceItemModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SteeringWheel.bin", NULL);

	m_ppHierarchicalGameObjects[14] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pIceItemModel, 0);

	m_ppHierarchicalGameObjects[14]->SetPosition(-12.0f, -200.0f, -220.f);
	m_ppHierarchicalGameObjects[14]->Rotate(45, 0, 0);
	m_ppHierarchicalGameObjects[14]->SetScale(60.0f, 60.0f, 60.0f);
	m_ppHierarchicalGameObjects[14]->isdraw = false;

	if (pIceItemModel) delete pIceItemModel;

	//PlayBGM(L"Sound/Ice.wav");

}

bool CIceScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		// �ʰ� �浹�� ���
		if (i == 0)
		{
			m_ppHierarchicalGameObjects[i]->m_bWasColliding = m_ppHierarchicalGameObjects[i]->m_bIsColliding;
			m_ppHierarchicalGameObjects[i]->m_bIsColliding = false; // 현재 프레임의 충돌 상태 초기화

			CGameObject* pMapObject = m_ppHierarchicalGameObjects[0]->m_pChild->m_pChild;

			for (int j = 0; j < m_ppHierarchicalGameObjects[0]->m_pChild->nChilds; j++)
			{
				const char* str = pMapObject->m_pstrFrameName;

				if (pMapObject->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
				{
					m_ppHierarchicalGameObjects[i]->m_bIsColliding = true;
					if (!strcmp(str, "bbfire"))
					{
						m_pPlayer->isFireMap = true;
						return true;
					}
					else if (!strcmp(str, "bbice"))
					{
						m_pPlayer->isIceMap = true;
						return true;
					}
					else if (!strcmp(str, "bbgrass"))
					{
						m_pPlayer->isNatureMap = true;
						return true;
					}

					// �ٴ��� �浹 üũ �ȵǵ���
					/*else */if (!strcmp(str, "Plane"))
						pMapObject = pMapObject->m_pSibling;

					else
						return(true);
				}
				pMapObject = pMapObject->m_pSibling;

				if (pMapObject == nullptr)break;
			}
		}

		// collision check with ice monster
		else if (i > 2 && i < 13 && m_ppHierarchicalGameObjects[i] != nullptr)
		{
			// monster with player's sword(attack mode)
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;

				gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Ice);

				return true;
			}
			// ice monster's hand with player (when ice monster attack)
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& (m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_xmBoundingBox)
					||
					m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				// 여기에 hp 닳는 코드 넣어주랑
				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 5);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
				g_clients[gNetwork.my_id].is_damage = true;
				gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);

				return false;
			}
		}
		// collision check with ice boss monster
		else if (i == 13)
		{
			// monster with player's sword
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;
				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 20);
				//gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Ice);

				return true;
			}
			//// ice boss hand with player
			//else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
			//	&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
			//	&& (m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_xmBoundingBox)
			//		||
			//		m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			//{
			//	// 여기에 hp 닳는 코드 넣어주랑
			//	m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
			//	m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;

			//	return false;
			//}
		}

		// collision check with ice item
		else if (i == 14) {
			XMFLOAT3 e_pos = m_ppHierarchicalGameObjects[i]->GetPosition();
			XMFLOAT3 p_pos = m_pPlayer->GetPosition();

			if (pow(p_pos.x - e_pos.x, 2) + pow(p_pos.y - e_pos.y, 2) + pow(p_pos.z - e_pos.z, 2) <= pow(10 + 10, 2))
			{
				isIceitem = true;
				m_ppHierarchicalGameObjects[i]->isdraw = false;
			}
		}
	}


	//if (m_ppHierarchicalGameObjects[0]->m_bWasColliding && !m_ppHierarchicalGameObjects[0]->m_bIsColliding) {
	//	// 충돌이 종료되었을 때 수행할 작업
	//	HandleCollisionEnd(m_ppHierarchicalGameObjects[0]);
	//}
	return(false);
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

	m_nHierarchicalGameObjects = 1 + 2 + 10 + 1 + 1; // map(1) + another player(2) + monster(10) + boss(1) + boss item(1)
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/firemap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 11.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);
	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 8);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 8);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;

	// 10개의 CLoadedModelInfo 인스턴스를 담을 배열
	CLoadedModelInfo* pFireEnemyModels[10];

	// 10개의 다른 모델을 로드
	for (int i = 0; i < 10; ++i) {
		pFireEnemyModels[i] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shade.bin", NULL);
	}

	// 각 모델에 대해 CPlayerObject를 생성
	for (int i = 0; i < 10; ++i) {
		m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFireEnemyModels[i], 6);

		// 애니메이션 트랙 설정 및 일부 트랙 비활성화
		for (int j = 0; j <= 5; ++j) {
			m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(j, j);
			m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(j, !(j == 0 || j == 2 || j == 3 || j == 4 || j == 5)); // 특정 트랙을 제외하고 모두 활성화
		}

		// 초기 속성 설정
		m_ppHierarchicalGameObjects[3 + i]->isdraw = false;

		float posX = (rand() % 2000) / 10.0f;
		float posY = (rand() % 2000) / 10.0f;

		m_ppHierarchicalGameObjects[3 + i]->SetPosition(posX, -200.0f, posY);
		m_ppHierarchicalGameObjects[3 + i]->SetScale(10.0f, 10.0f, 10.0f);
		m_ppHierarchicalGameObjects[3 + i]->SetMonsType(MONSTERTYPE::FIRE);
	}

	// 로드된 모든 모델 해제
	for (int i = 0; i < 10; ++i) {
		if (pFireEnemyModels[i]) delete pFireEnemyModels[i];
	}

	CLoadedModelInfo* pFireBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Shadow.bin", NULL);

	m_ppHierarchicalGameObjects[13] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFireBossModel, 7);

	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);

	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppHierarchicalGameObjects[13]->SetPosition(410.0f, -200.0f, 735.0f);
	m_ppHierarchicalGameObjects[13]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[13]->SetMonsType(MONSTERTYPE::FIREBOSS);
	if (pFireBossModel) delete pFireBossModel;

	CLoadedModelInfo* pFireItemModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/FuelTank.bin", NULL);

	m_ppHierarchicalGameObjects[14] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFireItemModel, 0);

	m_ppHierarchicalGameObjects[14]->SetPosition(-60.0, -200.0f, 1327.0f);
	m_ppHierarchicalGameObjects[14]->Rotate(90, 90, 0);
	m_ppHierarchicalGameObjects[14]->SetScale(60.0f, 60.0f, 60.0f);
	m_ppHierarchicalGameObjects[14]->isdraw = false;

	if (pFireItemModel) delete pFireItemModel;
}
bool CFireScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
	
		// �ʰ� �浹�� ���
		if (i == 0)
		{
			m_ppHierarchicalGameObjects[i]->m_bWasColliding = m_ppHierarchicalGameObjects[i]->m_bIsColliding;
			m_ppHierarchicalGameObjects[i]->m_bIsColliding = false; // 현재 프레임의 충돌 상태 초기화

			CGameObject* pMapObject = m_ppHierarchicalGameObjects[0]->m_pChild->m_pChild;

			for (int j = 0; j < m_ppHierarchicalGameObjects[0]->m_pChild->nChilds; j++)
			{
				const char* str = pMapObject->m_pstrFrameName;

				if (pMapObject->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
				{
					m_ppHierarchicalGameObjects[i]->m_bIsColliding = true;
					if (!strcmp(str, "bbfire"))
					{
						m_pPlayer->isFireMap = true;
						return true;
					}
					else if (!strcmp(str, "bbice"))
					{
						m_pPlayer->isIceMap = true;
						return true;
					}
					else if (!strcmp(str, "bbgrass"))
					{
						m_pPlayer->isNatureMap = true;
						return true;
					}

					// �ٴ��� �浹 üũ �ȵǵ���
					/*else */if (!strcmp(str, "Plane"))
						pMapObject = pMapObject->m_pSibling;

					else
						return(true);
				}
				pMapObject = pMapObject->m_pSibling;

				if (pMapObject == nullptr)break;
			}
		}

		// collision check with fire monster
		else if (i > 2 && i < 13 && m_ppHierarchicalGameObjects[i] !=  nullptr)
		{
			// monster with player's sword(attack mode)
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild
					->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;
				//// send  
				gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Fire);

				return true;
			}
			// fire monster's hand with player (when firemonster attack)
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				// 왼손
				&& (m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_xmBoundingBox)
					||
					m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				// 여기에 hp 닳는 코드 넣어주랑
				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 5);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
				g_clients[gNetwork.my_id].is_damage = true;
				gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);
				return false;
			}
		}
		// collision check with fire boss monster
		else if (i == 13)
		{
			// fire boss with player(attack mode)
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox)
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0)
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;

				// 보스 체력 서버로 전송 
				gNetwork.SendBossDamage(g_FireBossMonster.getHp() - g_clients[gNetwork.my_id].getAttackPower(), MonsterType::Fire_Boss);

				return true;
			}
			// fire boss hand with player
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& (m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_xmBoundingBox)
					||
					m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_pChild->m_xmBoundingBox)))
			{
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;

				g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 20);
				g_clients[gNetwork.my_id].is_damage = true;
				gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);

				return false;
			}
		}

		// collision check with fire item
		else if (i == 14) {
			XMFLOAT3 e_pos = m_ppHierarchicalGameObjects[i]->GetPosition();
			XMFLOAT3 p_pos = m_pPlayer->GetPosition();

			if (pow(p_pos.x - e_pos.x, 2) + pow(p_pos.y - e_pos.y, 2) + pow(p_pos.z - e_pos.z, 2) <= pow(10 + 10, 2))
			{
				isFireitem = true;
				m_ppHierarchicalGameObjects[i]->isdraw = false;
			}
		}
	}


	if (m_ppHierarchicalGameObjects[0]->m_bWasColliding && !m_ppHierarchicalGameObjects[0]->m_bIsColliding) {
		// 충돌이 종료되었을 때 수행할 작업
		HandleCollisionEnd(m_ppHierarchicalGameObjects[0]);
	}
	return(false);
}
void CFireScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CFireScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGrassScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 1 + 2 + 10 + 1 + 1; // map(1) + another player(2) + monster(10) + boss(1) + boss item(1)
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/grassmap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(280.0f, 11.0f, 620.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 5.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);
	CLoadedModelInfo* pPlayerModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SK_Mesh_Astronaut_sword.bin", NULL);

	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel1, 8);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[1]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel2, 8);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7);

	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(6, false);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_ppHierarchicalGameObjects[2]->SetPosition(410.0f, -50.0f, 735.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);

	if (pPlayerModel1) delete pPlayerModel1;
	if (pPlayerModel2) delete pPlayerModel2;

	// 10개의 CLoadedModelInfo 인스턴스를 담을 배열
	CLoadedModelInfo* pGrassEnemyModels[10];

	// 10개의 다른 모델을 로드
	for (int i = 0; i < 10; ++i) {
		pGrassEnemyModels[i] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Slime_Cube.bin", NULL);
	}

	// 각 모델에 대해 CPlayerObject를 생성
	for (int i = 0; i < 10; ++i) {
		m_ppHierarchicalGameObjects[3 + i] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pGrassEnemyModels[i], 6);

		// 애니메이션 트랙 설정 및 일부 트랙 비활성화
		for (int j = 0; j <= 5; ++j) {
			m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackAnimationSet(j, j);
			m_ppHierarchicalGameObjects[3 + i]->m_pSkinnedAnimationController->SetTrackEnable(j, !(j == 0 || j == 2 || j == 3 || j == 4 || j == 5)); // 특정 트랙을 제외하고 모두 활성화
		}

		// 초기 속성 설정
		m_ppHierarchicalGameObjects[3 + i]->isdraw = false;

		float posX = (rand() % 2000) / 10.0f;
		float posY = (rand() % 2000) / 10.0f;

		m_ppHierarchicalGameObjects[3 + i]->SetPosition(posX, -200.0f, posY);
		m_ppHierarchicalGameObjects[3 + i]->SetScale(10.0f, 10.0f, 10.0f);
		m_ppHierarchicalGameObjects[3 + i]->SetMonsType(MONSTERTYPE::NATURE);
	}

	// 로드된 모든 모델 해제
	for (int i = 0; i < 10; ++i) {
		if (pGrassEnemyModels[i]) delete pGrassEnemyModels[i];
	}

	CLoadedModelInfo* pGrassBossModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Slime_Monster.bin", NULL);

	m_ppHierarchicalGameObjects[13] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pGrassBossModel, 6);

	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5);

	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppHierarchicalGameObjects[13]->m_pSkinnedAnimationController->SetTrackEnable(5, false);

	m_ppHierarchicalGameObjects[13]->SetPosition(410.0f, -200.0f, 735.0f);
	m_ppHierarchicalGameObjects[13]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[13]->SetMonsType(MONSTERTYPE::NATUREBOSS);
	if (pGrassBossModel) delete pGrassBossModel;

	CLoadedModelInfo* pNatureItemModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Wheel.bin", NULL);

	m_ppHierarchicalGameObjects[14] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pNatureItemModel, 0);

	m_ppHierarchicalGameObjects[14]->SetPosition(77.0f, -200.0f, -408.0f);
	m_ppHierarchicalGameObjects[14]->SetScale(60.0f, 60.0f, 60.0f);
	m_ppHierarchicalGameObjects[14]->isdraw = false;

	if (pNatureItemModel) delete pNatureItemModel;
}
bool CGrassScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		// �ʰ� �浹�� ���
		if (i == 0)
		{
			m_ppHierarchicalGameObjects[i]->m_bWasColliding = m_ppHierarchicalGameObjects[i]->m_bIsColliding;
			m_ppHierarchicalGameObjects[i]->m_bIsColliding = false; // 현재 프레임의 충돌 상태 초기화

			CGameObject* pMapObject = m_ppHierarchicalGameObjects[0]->m_pChild->m_pChild;

			for (int j = 0; j < m_ppHierarchicalGameObjects[0]->m_pChild->nChilds; j++)
			{
				const char* str = pMapObject->m_pstrFrameName;

				if (pMapObject->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox))
				{
					m_ppHierarchicalGameObjects[i]->m_bIsColliding = true;
					if (!strcmp(str, "bbfire"))
					{
						m_pPlayer->isFireMap = true;
						return true;
					}
					else if (!strcmp(str, "bbice"))
					{
						m_pPlayer->isIceMap = true;
						return true;
					}
					else if (!strcmp(str, "bbgrass"))
					{
						m_pPlayer->isNatureMap = true;
						return true;
					}

					// �ٴ��� �浹 üũ �ȵǵ���
					/*else */if (!strcmp(str, "Plane"))
						pMapObject = pMapObject->m_pSibling;

					else
						return(true);
				}
				pMapObject = pMapObject->m_pSibling;

				if (pMapObject == nullptr)break;
			}
		}

		// collision check with nature monster
		else if (i > 2 && i < 13 && m_ppHierarchicalGameObjects[i] != nullptr)
		{
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsValidAttack == true
				&& m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack == 0
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				m_pPlayer->m_pSkinnedAnimationController->m_nCntValidAttack++;

				gNetwork.SendMonsterDie(g_monsters[i - 3].getId(), MonsterType::Nature);

				return true;
			}
			// grass monster's head with player (when grass monster attack)
			else if (m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bMonsterValidAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt == 0
				&& m_pPlayer->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_pSibling->m_pChild->m_xmBoundingBox))
			{
				// 여기에 hp 닳는 코드 넣어주랑
				//g_clients[gNetwork.my_id].setHp(g_clients[gNetwork.my_id].getHp() - 5);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_nMonsterAttackCnt++;
				m_pPlayer->m_pSkinnedAnimationController->m_bIsPlayerAttacked = true;
				//g_clients[gNetwork.my_id].is_damage = true;
				//gNetwork.SendPlayerHIt(g_clients[gNetwork.my_id].is_damage);
				return false;
			}
		}
		// collision check with nature boss monster
		else if (i == 13)
		{
			// monster with player(attack mode)
			if (m_pPlayer->m_pSkinnedAnimationController->m_bIsAttack == true
				&& m_ppHierarchicalGameObjects[i]->m_pChild->m_pChild->m_xmBoundingBox.Intersects(m_pPlayer->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pChild->m_pChild->m_pChild->m_pChild->m_pSibling->m_pSibling->m_pSibling->m_xmBoundingBox))
			{
				//send attacked monster num
				CAnimationSet* pAnimationSet = m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_pAnimationTracks[4].m_nAnimationSet];
				float fPosition2 = m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_pAnimationTracks[4].UpdatePosition(m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_pAnimationTracks[4].m_fPosition, m_fElapsedTime, pAnimationSet->m_fLength);

				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->SetTrackEnable(3, true);
				m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController->m_bIsAttacked = true;

				return(true);
			}
		}

		// collision check with nature item
		else if (i == 14 ) {
			XMFLOAT3 e_pos = m_ppHierarchicalGameObjects[i]->GetPosition();
			XMFLOAT3 p_pos = m_pPlayer->GetPosition();

			if (pow(p_pos.x - e_pos.x, 2) + pow(p_pos.y - e_pos.y, 2) + pow(p_pos.z - e_pos.z, 2) <= pow(10 + 10, 2))
			{
				isNatureitem = true;
				m_ppHierarchicalGameObjects[i]->isdraw = false;
			}
		}
	}


	if (m_ppHierarchicalGameObjects[0]->m_bWasColliding && !m_ppHierarchicalGameObjects[0]->m_bIsColliding) {
		// 충돌이 종료되었을 때 수행할 작업
		HandleCollisionEnd(m_ppHierarchicalGameObjects[0]);
	}
	return(false);
}

void CGrassScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CGrassScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

void CWInScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 4;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/win.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(200.0f, -10.0f, 400.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(5.0f, 3.0f, 5.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/dance.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[1]->SetPosition(50.0f, 0.0f, -15.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(30.0f, 30.0f, 30.0f);
	m_ppHierarchicalGameObjects[1]->Rotate(-20.0f, 170.0f, 00.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[2]->SetPosition(0.0f, 0.0f, -15.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(30.0f, 30.0f, 30.0f);
	m_ppHierarchicalGameObjects[2]->Rotate(-20.0f, 170.0f, 00.0f);

	m_ppHierarchicalGameObjects[3] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[3]->SetPosition(-50.0f, 0.0f, -15.0f);
	m_ppHierarchicalGameObjects[3]->SetScale(30.0f, 30.0f, 30.0f);
	m_ppHierarchicalGameObjects[3]->Rotate(-20.0f, 170.0f, 0.0f);

	if (pPlayerModel) delete pPlayerModel;
}
void CWInScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CWInScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

void CLoseScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nHierarchicalGameObjects = 4;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* map = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Map/spaceshipmap.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, map, 0);
	m_ppHierarchicalGameObjects[0]->SetPosition(100.0f, -100.0f, -100.0f);
	m_ppHierarchicalGameObjects[0]->SetScale(10.0f, 8.0f, 10.0f);
	if (map) delete map;

	CLoadedModelInfo* pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/dance.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	m_ppHierarchicalGameObjects[1]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[1]->SetPosition(-100.0f, 0.0f, -45.0f);
	m_ppHierarchicalGameObjects[1]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[1]->Rotate(-20.0f, 170.0f, 00.0f);

	m_ppHierarchicalGameObjects[2] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	m_ppHierarchicalGameObjects[2]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[2]->SetPosition(20.0f, 0.0f, -55.0f);
	m_ppHierarchicalGameObjects[2]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[2]->Rotate(-20.0f, 170.0f, 00.0f);

	m_ppHierarchicalGameObjects[3] = new CPlayerObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pPlayerModel, 1);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 3);
	m_ppHierarchicalGameObjects[3]->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.3);
	m_ppHierarchicalGameObjects[3]->SetScale(20.0f, 20.0f, 20.0f);
	m_ppHierarchicalGameObjects[3]->Rotate(0.0f, 0.0f, 0.0f);
	m_ppHierarchicalGameObjects[3]->SetPosition(-60.0f, 0.0f, -30.0f);

	if (pPlayerModel) delete pPlayerModel;
}
void CLoseScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
}

void CLoseScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}