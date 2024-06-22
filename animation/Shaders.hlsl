struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix					gmtxView : packoffset(c0);
	matrix					gmtxProjection : packoffset(c4);
	float3					gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix					gmtxGameObject : packoffset(c0);
	MATERIAL				gMaterial : packoffset(c4);
	uint					gnTexturesMask : packoffset(c8);
};

#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#define FRAME_BUFFER_WIDTH				640
#define FRAME_BUFFER_HEIGHT				480

Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);

SamplerState gssWrap : register(s0);

// Sobel Outline Compute
float4 Sobel(Texture2D tex, float2 uv, SamplerState sam)
{
    float2 texelSize = 1.0f / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    float4 tl = tex.Sample(sam, uv + float2(-texelSize.x, -texelSize.y));
    float4 tc = tex.Sample(sam, uv + float2(0, -texelSize.y));
    float4 tr = tex.Sample(sam, uv + float2(texelSize.x, -texelSize.y));
    float4 cl = tex.Sample(sam, uv + float2(-texelSize.x, 0));
    float4 cr = tex.Sample(sam, uv + float2(texelSize.x, 0));
    float4 bl = tex.Sample(sam, uv + float2(-texelSize.x, texelSize.y));
    float4 bc = tex.Sample(sam, uv + float2(0, texelSize.y));
    float4 br = tex.Sample(sam, uv + float2(texelSize.x, texelSize.y));

    float4 gx = -1 * tl + -2 * cl + -1 * bl + 1 * tr + 2 * cr + 1 * br;
    float4 gy = -1 * tl + -2 * tc + -1 * tr + 1 * bl + 2 * bc + 1 * br;

    float4 g = sqrt(gx * gx + gy * gy);

    return g;
}

struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
};

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
    float4 scene : SV_Target0;
	
    float4 cTexture : SV_TARGET1;
    float4 diffuse : SV_TARGET2;
    float4 normal : SV_TARGET3;
    float4 zDepth : SV_TARGET4;
};

Texture2DArray gtxtTextureArray : register(t19);

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
	output.bitangentW = mul(input.bitangent, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedStandardMultipleRTs(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    
	// 객체 렌더링
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
	
    output.cTexture = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
	
    output.diffuse = gMaterial.m_cDiffuse;
    
    output.normal = float4(input.normalW, 0.0);
    
    output.zDepth = float4(input.position.z, 0.0f, input.position.z, 1.0);
	
	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 정점 하나에 영향을 줄 수 잇는 뼈의 개수는 4개
#define MAX_VERTEX_INFLUENCES			4
// 계층구조에 있는 애니메이션을 할 수 있는 뼈의 수는 256개
#define SKINNED_ANIMATION_BONES			256

cbuffer cbBoneOffsets : register(b7)
{
	// 의미: 모델좌표계에 있는 어떤 정점을 어떠한 뼈에 대한 정보로 변경
	// 각 뼈에대한 정보
	float4x4 gpmtxBoneOffsets[SKINNED_ANIMATION_BONES];
};

cbuffer cbBoneTransforms : register(b8)
{
	float4x4 gpmtxBoneTransforms[SKINNED_ANIMATION_BONES];
};

struct VS_SKINNED_STANDARD_INPUT
{
	// 
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	int4 indices : BONEINDEX;
	float4 weights : BONEWEIGHT;
};

VS_STANDARD_OUTPUT VSSkinnedAnimationStandard(VS_SKINNED_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	// 주석 부분이나 아래 코드나 같아요
	//output.positionW = float3(0.0f, 0.0f, 0.0f);
	//output.normalW = float3(0.0f, 0.0f, 0.0f);
	//output.tangentW = float3(0.0f, 0.0f, 0.0f);
	//output.bitangentW = float3(0.0f, 0.0f, 0.0f);
	//matrix mtxVertexToBoneWorld;
	//for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	//{
	//	mtxVertexToBoneWorld = mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	//	output.positionW += input.weights[i] * mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	//	output.normalW += input.weights[i] * mul(input.normal, (float3x3)mtxVertexToBoneWorld);
	//	output.tangentW += input.weights[i] * mul(input.tangent, (float3x3)mtxVertexToBoneWorld);
	//	output.bitangentW += input.weights[i] * mul(input.bitangent, (float3x3)mtxVertexToBoneWorld);
	//}
	float4x4 mtxVertexToBoneWorld = (float4x4)0.0f;
	for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
	{
		// 하나의 정점에 영향을 주는 뼈들이 4개, 이 4개에 대해 처리해주는 부분. 4개의 뼈에 해당하는 인덱스에 해당하는 본오프셋하고 본트랜스폼 행렬하고 곱해서 각 행렬들의 weight를 곱해서... 
//		mtxVertexToBoneWorld += input.weights[i] * gpmtxBoneTransforms[input.indices[i]];
		mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
	}
	output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
	output.normalW = mul(input.normal, (float3x3)mtxVertexToBoneWorld).xyz;
	output.tangentW = mul(input.tangent, (float3x3)mtxVertexToBoneWorld).xyz;
	output.bitangentW = mul(input.bitangent, (float3x3)mtxVertexToBoneWorld).xyz;

//	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;

	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}




PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingToMultipleRTs(VS_STANDARD_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	
    //output.cTexture = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    
    
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_EMISSION_MAP)
        cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
    
    
    output.cTexture = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
	
	output.normal = float4(input.normalW.xyz * 0.5f + 0.5f, 1.0f);
    //output.normal = float4(0.0,0.0,0.0, 1.0f);

    input.normalW = normalize(input.normalW);

   // output.zDepth = input.position.z;
    output.zDepth = float4(input.position.z, 0.0f,input.position.z, 1.0);
	
    output.diffuse = gMaterial.m_cDiffuse;
    //output.diffuse = float4(1.0, 1.0, 1.0, 1.0);
	
    float4 cIllumination = Lighting(input.positionW, input.normalW);
	
    output.cTexture = lerp(output.cTexture, cIllumination, 0.5f);
    cIllumination = gMaterial.m_cDiffuse;
    
    return (output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainBaseTexture : register(t1);
Texture2D gtxtTerrainDetailTexture : register(t2);

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gssWrap, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gssWrap, input.uv1);
//	float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
	float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

TextureCube gtxtSkyCubeTexture : register(t13);
SamplerState gssClamp : register(s1);

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);
    
    return (cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float4 VSPostProcessing(uint nVertexID : SV_VertexID) : SV_POSITION
{
    if (nVertexID == 0)
        return (float4(-1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 1)
        return (float4(+1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 2)
        return (float4(+1.0f, -1.0f, 0.0f, 1.0f));

    if (nVertexID == 3)
        return (float4(-1.0f, +1.0f, 0.0f, 1.0f));
    if (nVertexID == 4)
        return (float4(+1.0f, -1.0f, 0.0f, 1.0f));
    if (nVertexID == 5)
        return (float4(-1.0f, -1.0f, 0.0f, 1.0f));

    return (float4(1.0, 1.0, 1.0, 1.0));
}

float4 PSPostProcessing(float4 position : SV_POSITION) : SV_Target
{
	
	
    return (float4(1.0f, 1.0f, 1.0f, 1.0f));
}

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D<float4> gtxtTextureTexture : register(t14);
Texture2D<float4> gtxtIlluminationTexture : register(t15);
Texture2D<float4> gtxtdrNormalTexture : register(t16);

Texture2D<float> gtxtzDepthTexture : register(t17);
Texture2D<float> gtxtDepthTexture : register(t18);


VS_SCREEN_RECT_TEXTURED_OUTPUT VSScreenRectSamplingTextured(uint nVertexID : SV_VertexID)
{
    VS_SCREEN_RECT_TEXTURED_OUTPUT output;

    if (nVertexID == 0)
    {
        output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    else if (nVertexID == 1)
    {
        output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 0.0f);
    }
    else if (nVertexID == 2)
    {
        output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    else if (nVertexID == 3)
    {
        output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    else if (nVertexID == 4)
    {
        output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    else if (nVertexID == 5)
    {
        output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 1.0f);
    }
	
	
    return (output);
}

float4 GetColorFromDepth(float fDepth)
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    if (fDepth >= 1.0f)
        cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    else if (fDepth < 0.00625f)
        cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (fDepth < 0.0125f)
        cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (fDepth < 0.025f)
        cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
    else if (fDepth < 0.05f)
        cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
    else if (fDepth < 0.075f)
        cColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
    else if (fDepth < 0.1f)
        cColor = float4(1.0f, 0.5f, 0.5f, 1.0f);
    else if (fDepth < 0.4f)
        cColor = float4(0.5f, 1.0f, 1.0f, 1.0f);
    else if (fDepth < 0.6f)
        cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
    else if (fDepth < 0.8f)
        cColor = float4(0.5f, 0.5f, 1.0f, 1.0f);
    else if (fDepth < 0.9f)
        cColor = float4(0.5f, 1.0f, 0.5f, 1.0f);
    else
        cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return (cColor);
}

cbuffer cbDrawOptions : register(b9)
{
    int4 gvDrawOptions : packoffset(c0);
};

float4 DeferredDirectionalLight(int nIndex, float3 vNormal, float3 vToCamera, float4 vSpecular, float4 vDiffuse, float4 vAmbient, float4 textureColor)
{
    float3 vToLight = -gLights[nIndex].m_vDirection;
    float fDiffuseFactor = dot(vToLight, vNormal);
    float fSpecularFactor = 0.0f;
    if (fDiffuseFactor > 0.0f)
    {
        if (vSpecular.a != 0.0f)
        {
#ifdef _WITH_REFLECT
            float3 vReflect = reflect(-vToLight, vNormal);
            fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), vSpecular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
            float3 vHalf = normalize(vToCamera + vToLight);
#else
            float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
            fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), vSpecular.a);
#endif
        }
    }

    float4 lightingColor = ((gLights[nIndex].m_cAmbient * vAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * vDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * vSpecular));

    // Sample the texture
    float4 textureSample = textureColor;
    
    // Set the output texture color
  //  textureColor = textureSample;

    // Combine the lighting color with the texture color
    float4 finalColor = lightingColor * textureSample;

    return finalColor;
}


float4 DeferredLighting(float3 vPosition, float3 vNormal, float4 vSpecular, float4 vDiffuse, float4 vAmbient, float4 tex)
{
    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	//float3 vCameraPosition = float3(0.0f, 0.0f, 0.0f);
    float3 vToCamera = normalize(vCameraPosition - vPosition);

    float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
    [unroll(MAX_LIGHTS)]
    for (int i = 0; i < gnLights; i++)
    {
        if (gLights[i].m_bEnable)
        {
            if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
            {
                cColor += DeferredDirectionalLight(i, vNormal, vToCamera, vSpecular, vDiffuse, vAmbient, tex);
            }
          
        }
    }
	
    cColor += (gcGlobalAmbientLight * vAmbient);
    cColor.a = vDiffuse.a;
	//cColor.a = 1.0f;

    return (cColor);
}

float4 PSScreenRectSamplingTextured(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
{
    float3 pos = input.position;
    float3 normal = gtxtdrNormalTexture.Sample(gssWrap, input.uv); // good
    float4 specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 diffuse = gMaterial.m_cDiffuse;
    float4 tex = gtxtTextureTexture.Sample(gssWrap, input.uv);
	
    float4 cColor = DeferredLighting(pos, normal, specular, diffuse, ambient, tex);
    
    switch (gvDrawOptions.x)
    {
        case 79: //'O'
		{
			// 끝
                cColor = gtxtTextureTexture.Sample(gssWrap, input.uv);
                //cColor = (1.0f, 0.0f, 0.0f, 1.0f);
                break;
            }
        case 78: //'N'
		{
			// 끝
                cColor = gtxtdrNormalTexture.Sample(gssWrap, input.uv);
                //cColor = gtxtIlluminationTexture.Sample(gssWrap, input.uv);
                break;
            }
        case 90: //'Z'
		{
                float fzDepth = gtxtzDepthTexture.Load(uint3((uint) input.position.x, (uint) input.position.y, 0));
                cColor = fzDepth;
                break;
            }
    }
    
    float4 cSobel = Sobel(gtxtTextureTexture, input.uv, gssWrap);
	
    cColor.rgb *= (1.0f - cSobel.r+0.3);
    
    return (cColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_BOUNDINGBOX_INPUT
{
    float3 position : POSITION;
};

struct VS_BOUNDINGBOX_OUTPUT
{
    float4 positionH : SV_POSITION;
};

VS_BOUNDINGBOX_OUTPUT VSBoundingBox(VS_BOUNDINGBOX_INPUT input)
{
    VS_BOUNDINGBOX_OUTPUT output;
    output.positionH = mul(mul(float4(input.position, 1.0f), gmtxView), gmtxProjection);
    return (output);
}

float4 PSBoundingBox(VS_BOUNDINGBOX_OUTPUT input) : SV_TARGET
{
    return (float4(1.0f, 0.0f, 0.0f, 1.0f));
}
