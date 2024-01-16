#include "pch.h"
#include "Effect.h"


Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);
	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pCameraPos = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPos->IsValid())
	{
		std::wcout << L"m_pCameraPos not valid!\n";
	}

	m_pWorldViewProjMatrix = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pWorldViewProjMatrix->IsValid())
	{
		std::wcout << L"m_pWorldViewProjMatrix not valid!\n";
	}

	m_pWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldViewProjMatrix->IsValid())
	{
		std::wcout << L"m_pWorldMatrix not valid!\n";
	}

	m_pDiffuseMap = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMap->IsValid())
	{
		std::wcout << L"m_pDiffuseMap not valid!\n";
	}

	m_pGlossinessMap = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMap->IsValid())
	{
		std::wcout << L"m_pGlossinessMap not valid!\n";
	}

	m_pSpecularMap = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMap->IsValid())
	{
		std::wcout << L"m_pSpecularMap not valid!\n";
	}

	m_pNormalMap = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMap->IsValid())
	{
		std::wcout << L"m_pNormalMap not valid!\n";
	}
}

Effect::~Effect()
{
	if (m_pEffect) 
	{
		if (m_pWorldViewProjMatrix) m_pWorldViewProjMatrix->Release();
		if (m_pDiffuseMap) m_pDiffuseMap->Release();
		m_pEffect->Release();
		m_pEffect = nullptr;
	}
	
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}
	return pEffect;
}


ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique()
{
	return m_pTechnique;
}

void Effect::SetWorldViewProjMatrix(const float* pData)
{
	m_pWorldViewProjMatrix->SetMatrix(pData);
}

void Effect::SetWorldMatrix(const float* pData)
{
	m_pWorldMatrix->SetMatrix(pData);
}

void Effect::SetCameraPos(const float* pData)
{
	m_pCameraPos->SetFloatVector(pData);
}

void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMap)
		m_pDiffuseMap->SetResource(pDiffuseTexture->GetTexture());
}

void Effect::SetGlossinessMap(Texture* pGlossinessTexture)
{
	if (m_pGlossinessMap)
		m_pGlossinessMap->SetResource(pGlossinessTexture->GetTexture());
}

void Effect::SetSpecularMap(Texture* pSpecularTexture)
{
	if (m_pSpecularMap)
		m_pSpecularMap->SetResource(pSpecularTexture->GetTexture());
}

void Effect::SetNormalMap(Texture* pNormalTexture)
{
	if (m_pNormalMap)
		m_pNormalMap->SetResource(pNormalTexture->GetTexture());
}
