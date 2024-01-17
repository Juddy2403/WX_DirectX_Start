#include "pch.h"
#include "ComplexEffect.h"


ComplexEffect::ComplexEffect(ID3D11Device* pDevice, const std::wstring& assetFile):
	Effect(pDevice, assetFile)
{
	m_pCameraPos = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPos->IsValid())
	{
		std::wcout << L"m_pCameraPos not valid!\n";
	}

	m_pAreNormalsToggled = m_pEffect->GetVariableByName("gAreNormalsToggled")->AsScalar();
	if (!m_pAreNormalsToggled->IsValid())
	{
		std::wcout << L"m_AreNormalsToggled not valid!\n";
	}
	m_pAreNormalsToggled->SetBool(m_NormalsBool);

	m_pWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldViewProjMatrix->IsValid())
	{
		std::wcout << L"m_pWorldMatrix not valid!\n";
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

ComplexEffect::~ComplexEffect()
{
	if (m_pWorldMatrix) m_pWorldMatrix->Release();
	if (m_pCameraPos) m_pCameraPos->Release();
	if (m_pNormalMap) m_pNormalMap->Release();
	if (m_pGlossinessMap) m_pGlossinessMap->Release();
	if (m_pSpecularMap) m_pSpecularMap->Release();
	if (m_pAreNormalsToggled) m_pAreNormalsToggled->Release();
}


void ComplexEffect::SetWorldMatrix(const float* pData)
{
	m_pWorldMatrix->SetMatrix(pData);
}

void ComplexEffect::SetCameraPos(const float* pData)
{
	m_pCameraPos->SetFloatVector(pData);
}

void ComplexEffect::ToggleNormals()
{
	m_NormalsBool = !m_NormalsBool;
	std::wcout << L"Is normal mapping active: " << std::boolalpha << m_NormalsBool << "\n";
	m_pAreNormalsToggled->SetBool(m_NormalsBool);
}


void ComplexEffect::SetGlossinessMap(Texture* pGlossinessTexture)
{
	if (m_pGlossinessMap)
		m_pGlossinessMap->SetResource(pGlossinessTexture->GetTexture());
}

void ComplexEffect::SetSpecularMap(Texture* pSpecularTexture)
{
	if (m_pSpecularMap)
		m_pSpecularMap->SetResource(pSpecularTexture->GetTexture());
}

void ComplexEffect::SetNormalMap(Texture* pNormalTexture)
{
	if (m_pNormalMap)
		m_pNormalMap->SetResource(pNormalTexture->GetTexture());
}
