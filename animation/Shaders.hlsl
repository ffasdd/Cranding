struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular; //a = power
    float4 m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    matrix gmtxShadowTransform : packoffset(c8); // 그림자 변환 행렬
    float3 gvCameraPosition : packoffset(c12);    
};

cbuffer cbGameObjectInfo : register(b2)
{
    matrix gmtxGameObject : packoffset(c0);
    MATERIAL gMaterial : packoffset(c4);
    uint gnTexturesMask : packoffset(c8);
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

////_FULLSCREEN
//#define FRAME_BUFFER_WIDTH				1920
//#define FRAME_BUFFER_HEIGHT				1080

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

// Laplacian Outline Compute
float4 Laplacian(Texture2D tex, float2 uv, SamplerState sam)
{
    float2 texelSize = 1.0f / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    float4 tl = tex.Sample(sam, uv + float2(-texelSize.x, -texelSize.y));
    float4 tc = tex.Sample(sam, uv + float2(0, -texelSize.y));
    float4 tr = tex.Sample(sam, uv + float2(texelSize.x, -texelSize.y));
    float4 cl = tex.Sample(sam, uv + float2(-texelSize.x, 0));
    float4 cc = tex.Sample(sam, uv + float2(0, 0)); // Center pixel
    float4 cr = tex.Sample(sam, uv + float2(texelSize.x, 0));
    float4 bl = tex.Sample(sam, uv + float2(-texelSize.x, texelSize.y));
    float4 bc = tex.Sample(sam, uv + float2(0, texelSize.y));
    float4 br = tex.Sample(sam, uv + float2(texelSize.x, texelSize.y));

    // Laplacian kernel
    // [ 0,  1, 0]
    // [ 1, -4, 1]
    // [ 0,  1, 0]
    float4 laplacian = 1 * tc + 1 * cl + -4 * cc + 1 * cr + 1 * bc;
    
   // float strength = 2.0f;
    //laplacian *= strength;

    return laplacian;
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
    float4 scene : SV_TARGET0;
	
    float4 f4Texture : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 PositionW : SV_TARGET3;
    float4 posW : SV_TARGET4;
};

Texture2DArray gtxtTextureArray : register(t19);

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.bitangentW = mul(input.bitangent, (float3x3) gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedStandardMultipleRTs(VS_STANDARD_OUTPUT input)
{
    // 이건 그냥 애들
    
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
    
	// 객체 렌더링
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
	
    output.f4Texture = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
	
    output.f4Texture = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        output.f4Texture = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    output.f4Texture.a = 1;

    
    input.normalW = normalize(input.normalW);
    output.normal = float4(input.normalW, 0);
    
    output.PositionW = float4(input.positionW, 1.0);
    
    output.posW = float4(input.positionW, 1.0f);
    //output.posW = float4(input.position.z, 0.0f, input.position.z, 1.0);
    //output.Position = float4(input.positionW, 0);
    
    output.scene = output.f4Texture + gMaterial.m_cEmissive;
    
    
    
    return (output);
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
    float4x4 mtxVertexToBoneWorld = (float4x4) 0.0f;
    for (int i = 0; i < MAX_VERTEX_INFLUENCES; i++)
    {
		// 하나의 정점에 영향을 주는 뼈들이 4개, 이 4개에 대해 처리해주는 부분. 4개의 뼈에 해당하는 인덱스에 해당하는 본오프셋하고 본트랜스폼 행렬하고 곱해서 각 행렬들의 weight를 곱해서... 
//		mtxVertexToBoneWorld += input.weights[i] * gpmtxBoneTransforms[input.indices[i]];
        mtxVertexToBoneWorld += input.weights[i] * mul(gpmtxBoneOffsets[input.indices[i]], gpmtxBoneTransforms[input.indices[i]]);
    }
    output.positionW = mul(float4(input.position, 1.0f), mtxVertexToBoneWorld).xyz;
    output.normalW = mul(input.normal, (float3x3) mtxVertexToBoneWorld).xyz;
    output.tangentW = mul(input.tangent, (float3x3) mtxVertexToBoneWorld).xyz;
    output.bitangentW = mul(input.bitangent, (float3x3) mtxVertexToBoneWorld).xyz;

//	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;

    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTexturedLightingToMultipleRTs(VS_STANDARD_OUTPUT input)
{
    // 이건 움직이는 애들
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	
    //output.f4Texture = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    
    
    //float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
    //    cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	
    //float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //if (gnTexturesMask & MATERIAL_SPECULAR_MAP)
    //    cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	
    //float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //if (gnTexturesMask & MATERIAL_NORMAL_MAP)
    //    cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	
    //float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //if (gnTexturesMask & MATERIAL_METALLIC_MAP)
    //    cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	
    //float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //if (gnTexturesMask & MATERIAL_EMISSION_MAP)
    //    cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
        
    //output.f4Texture = cAlbedoColor + cSpecularColor + cMetallicColor + cEmissionColor;
    output.f4Texture = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (gnTexturesMask & MATERIAL_ALBEDO_MAP)
        output.f4Texture = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    
    output.f4Texture.a = 1;
    
    //output.normal = float4(0.0,0.0,0.0, 1.0f);

	//output.normal = float4(input.normalW.xyz * 0.5f + 0.5f, 1.0f);
    //output.normal = float4(input.normalW, 0);
    //output.posW = input.position.z;
    //output.posW = float4(input.position.z, 0.0f,input.position.z, 1.0);
    
    output.normal = float4(input.normalW, 0);
    input.normalW = normalize(input.normalW);
    
    output.PositionW = float4(input.positionW, 0.0);
    //output.diffuse = float4(1.0, 1.0, 1.0, 1.0);
	
    //output.posW = input.position.z;
    //output.posW = float4(input.position.z, 0.0f,input.position.z, 1.0);
    // output.Position = float4(input.positionW, 0);
    
    //float4 cIllumination = Lighting(input.positionW, input.normalW);
	
    //output.f4Texture = lerp(output.f4Texture, cIllumination, 0.5f);
    //cIllumination = gMaterial.m_cDiffuse;
    output.posW = float4(input.positionW, 0.0f);
    output.scene = output.f4Texture + gMaterial.m_cEmissive;
    
    return (output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
    float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
    float3 positionL : POSITION;
    float4 position : SV_POSITION;
};

TextureCube gtxtSkyBoxTextureDay : register(t13);
TextureCube gtxtSkyBoxTextureNight : register(t1);

SamplerState gssClamp : register(s1);

Texture2D gtxtInputTextures[5] : register(t14); //To Defferd Rendering

Texture2D gtxShadowMap : register(t2);

SamplerComparisonState gssComparisonPCFShadow : register(s2);
cbuffer cbBlendFactor : register(b10)
{
    float g_fMin;
    float g_fSec;
};

float4 BlendSkyTextures(float3 direction)
{
    float4 dayColor = gtxtSkyBoxTextureDay.Sample(gssWrap, direction);
    float4 nightColor = gtxtSkyBoxTextureNight.Sample(gssWrap, direction);

    // Total time in seconds within the 5-minute cycle
    float totalSeconds = g_fMin * 60.0 + g_fSec;

    // Calculate the blend factor based on the total time
    float blendFactor = 0.0f;
    if (totalSeconds <= 30.0) // First 30 sec
    {
        blendFactor = totalSeconds / 30.0; // Blend from 0 to 1
    }
    else // Last 1 minutes
    {
        blendFactor = (90.0 - totalSeconds) / 60.0; // Blend from 1 to 0
    }

    return lerp(nightColor, dayColor, blendFactor);
}

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
    VS_SKYBOX_CUBEMAP_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.positionL = input.position;

    
    return (output);
}

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
    float3 direction = input.positionL;
    float4 cColor = BlendSkyTextures(direction);
    //cColor = float4(.0, .0, 1.0, 0.0);
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

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    
    float3 viewSpaceDir : TEXCOORD1;
};

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
	
    output.viewSpaceDir = mul(output.position, gmtxProjection).xyz;
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

float2 GenerateOffset(int index, int kernelSize, float texelSize)
{
    int halfSize = kernelSize / 2;
    int x = index % kernelSize;
    int y = index / kernelSize;
    return float2((x - halfSize) * texelSize, (y - halfSize) * texelSize);
}

float CalcShadowFactor(float4 shadowPosH, float bias)
{

    shadowPosH.xyz /= shadowPosH.w;

    float depth = shadowPosH.z - 0.001;

    uint width, height, numMips;
    gtxShadowMap.GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float) width;
    int kernelSize = 3;
    
    float percentLit = 0.0f;
    
    [unroll]
    for (int i = 0; i < kernelSize * kernelSize; ++i)
    {
        float2 offset = GenerateOffset(i, kernelSize, dx);
        percentLit += gtxShadowMap.SampleCmpLevelZero(gssComparisonPCFShadow, shadowPosH.xy + offset, depth + bias).r;
    }

    return percentLit / (float) (kernelSize * kernelSize);

}

float4 PSScreenRectSamplingTextured(VS_SCREEN_RECT_TEXTURED_OUTPUT input) : SV_Target
{
    float2 textureSize = float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    float2 texelCoord = input.uv * textureSize;
    uint3 texCoord = uint3(texelCoord, 0);
    
    float4 tex = gtxtInputTextures[0].Load(texCoord);
    float3 normal = gtxtInputTextures[1].Load(texCoord).rgb; 
    float4 position = gtxtInputTextures[2].Load(texCoord);
    //return position;
    float4 specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 diffuse = gMaterial.m_cDiffuse;  
    
    float3 pos = position.xyz;
    matrix shadowTransformMatrix =
    {
        -0.0176776703f, 0.00000000f, 0.0176776703f, 1.29549515f,
        0.0158168618f, -0.0111648431f, 0.0158168618f, 1.36992741f,
        -0.00789473671, -0.0223684218f, -0.00789473671, 0.0657894909,
        0.00000000f, 0.00000000f, 0.00000000f, 1.00000000f
    };
    float4 ShadowPosH = mul(float4(gtxtInputTextures[2][int2(input.position.xy)].xyz, 1.0f), gmtxShadowTransform);
    float biasValue = 0.000f;
    float ShadowFactor = CalcShadowFactor(ShadowPosH, biasValue);
    

    if (ShadowPosH.x > 1 || ShadowPosH.y > 1 || ShadowPosH.z > 1 || ShadowPosH.x < 0 || ShadowPosH.y < 0 || ShadowPosH.z < 0)
    {
        ShadowFactor = 1.f;
        //return float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    ShadowFactor += 0.5f;

    ShadowFactor = saturate(ShadowFactor);

    
    float4 Illumination = DeferredLighting(pos, normal, specular, diffuse, ambient, tex);
    
   
    float4 cColor = tex;
    if (cColor.b >= 0.3f && cColor.r <= 0.1f && cColor.g <= 0.1f)
        discard;
    cColor.a = 1.0;
    
    bool isTexture;
    if (max(cColor.r, max(cColor.g, cColor.b)) != 0.f)
        isTexture = true;
    else
        isTexture = false;

    if (isTexture)
        cColor = lerp(cColor, Illumination, 0.5f) * ShadowFactor;
    else
        cColor = lerp(cColor, Illumination, 1.0f) * ShadowFactor;

    
    float4 cSobel = Sobel(gtxtInputTextures[0], input.uv, gssWrap);
	
    cColor.rgb *= (1.0f - cSobel.r + 0.3);

    switch (gvDrawOptions.x)
    {
        case 79: //'O'
		{
			// 끝
                //cColor = gtxtTextureTexture.Sample(gssWrap, input.uv);
                //cColor = (1.0f, 0.0f, 0.0f, 1.0f);
                break;
            }
        case 78: //'N'
		{
			// 끝
                //cColor = gtxtdrNormalTexture.Sample(gssWrap, input.uv);
                //cColor = gtxtIlluminationTexture.Sample(gssWrap, input.uv);
                break;
            }
        case 90: //'Z'
		{
                // fzDepth = gtxtzDepthTexture.Load(uint3((uint) input.position.x, (uint) input.position.y, 0));
                //cColor = fzDepth;
                break;
            }
    }
    

    
    //float4 la = Laplacian(gtxtdrNormalTexture, input.uv, gssWrap);
    //cColor.rgb *= (1.0f - la.r*4.0f + 0.3);
    //cColor.rgb *= la.r;
    
    //float outlineStrength = saturate(1.0f - la.r + 0.3f); // Ensure the value is clamped between 0 and 1
    //cColor.rgb = lerp(cColor.rgb, float3(0, 0, 0), outlineStrength); // Blend with black based on edge strength
    
     // Create an edge mask
    //float edgeThreshold = 0.1f; // Adjust this threshold as needed
    //float edgeMask = step(edgeThreshold, la.r);

    // Combine the original color with the edge overlay
    //float4 edgeColor = float4(0, 0, 0, 1); // Red color for the edge
    //float4 result = lerp(cColor, edgeColor, edgeMask);
    
    float ShadowMap = gtxShadowMap.Sample(gssWrap, input.uv).r;
    
    //return ShadowMap;
    return cColor;
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
/////////////////////////////////////////////////////////////////////
//
static float gfGaussianBlurMask2D[5][5] =
{
    { 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f },
    { 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
    { 7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f },
    { 4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f },
    { 1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f }
};

float4 GaussianBlur(float2 texCoord, float blurStrength)
{
    const int maxBlurRadius = 10;
    int blurRadius = int(blurStrength * maxBlurRadius);; // Radius of the blur kernel
    blurRadius = clamp(blurRadius, 0, maxBlurRadius);
   
   // Accumulate the weighted sum of colors
    float4 blurredColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;
    float2 textureSize = float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT); // Dimensions of the input texture
    float2 pixelSize = 1.0 / textureSize;

   // Sample the input texture multiple times within the blur radius
    for (int x = -blurRadius; x <= blurRadius; x++)
    {
        for (int y = -blurRadius; y <= blurRadius; y++)
        {
            float2 offset = float2(x, y) * pixelSize;
            float2 uv = texCoord + offset;

            float4 color = gtxtAlbedoTexture.Sample(gssWrap, uv);

         // Calculate the Gaussian weight based on the distance from the center pixel
            float distance = length(offset);
            float weight = exp(-(distance * distance) / (2 * blurRadius * blurRadius));

            blurredColor += color * weight;
            totalWeight += weight;
        }
    }

   // Normalize the accumulated color by the total weight
    blurredColor /= totalWeight;

    return blurredColor;
}

float4 PSBlur(float4 position : SV_POSITION) : SV_Target
{
    // 주어진 화면 좌표에서 알베도 텍스처의 색상을 가져옴
    float4 cColor = gtxtAlbedoTexture[int2(position.xy)];
    // 화면 공간의 위치를 텍스처 좌표로 변환 (uv)
    float2 texCoord = position.xy / float2(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    
    // gtxtzDepthTexture에서 텍스처 좌표에 해당하는 샘플을 가져옴
    //float4 objInfo = gtxtzDepthTexture.Sample(gssWrap, texCoord);
    
    //float depth = objInfo.a;
    //float brightness = dot(cColor.rgb, float3(0.299, 0.587, 0.114));
    //float blurStrength = 0.5;
    //if (brightness > 0.7f && depth >= 0.01f)
    //{
    //    cColor = GaussianBlur(texCoord, blurStrength);
    //}
    //else if (depth >= 0.00000001 && depth <= 0.01)
    //{
    //    blurStrength = 0.5f;
    //    cColor = GaussianBlur(texCoord, blurStrength);
    //}
    
    
    float brightness = dot(cColor.rgb, float3(0.299, 0.587, 0.114));
    float blurStrength = 0.5;
    if (brightness > 0.7f)
    {
        cColor = GaussianBlur(texCoord, blurStrength);
    }
    else
    {
        blurStrength = 0.5f;
        cColor = GaussianBlur(texCoord, blurStrength);
    }

    return cColor;
}

struct VS_ShadowMap_In
{
    float3 PosL : POSITION;
	//float2 TexC    : TEXCOORD;
};

struct VS_ShadowMap_Out
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};

VS_ShadowMap_Out VSShadowMap(VS_ShadowMap_In vin)
{
    VS_ShadowMap_Out vout = (VS_ShadowMap_Out) 0.0f;

	//MaterialData matData = gMaterialData[gMaterialIndex];

	// Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gmtxGameObject);

	// Transform to homogeneous clip space.
    vout.PosH = mul(mul(posW, gmtxView), gmtxProjection);
	// Output vertex attributes for interpolation across triangle.
	/*float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;*/
	
    return vout;
}

// This is only used for alpha cut out geometry, so that shadows 
// show up correctly.  Geometry that does not need to sample a
// texture can use a NULL pixel shader for depth pass.
void PSShadowMap(VS_ShadowMap_Out pin)
{
	//	// Fetch the material data.
	//	MaterialData matData = gMaterialData[gMaterialIndex];
	//	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	//	uint diffuseMapIndex = matData.DiffuseMapIndex;
	//
	//	// Dynamically look up the texture in the array.
	//	diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	//
	//#ifdef ALPHA_TEST
	//	// Discard pixel if texture alpha < 0.1.  We do this test as soon 
	//	// as possible in the shader so that we can potentially exit the
	//	// shader early, thereby skipping the rest of the shader code.
	//	clip(diffuseAlbedo.a - 0.1f);
	//#endif
}