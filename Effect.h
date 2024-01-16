#pragma once
#include "Texture.h"

class Effect final
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();
	ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique();
	void SetWorldViewProjMatrix(const float* pData);
	void SetWorldMatrix(const float* pData);
	void SetCameraPos(const float* pData);

	void SetDiffuseMap(Texture* pDiffuseTexture);
	void SetGlossinessMap(Texture* pGlossinessTexture);
	void SetSpecularMap(Texture* pSpecularTexture);
	void SetNormalMap(Texture* pNormalTexture);
private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pWorldViewProjMatrix;
	ID3DX11EffectMatrixVariable* m_pWorldMatrix;
	ID3DX11EffectVectorVariable* m_pCameraPos;

	//2DTexture maps
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;
	ID3DX11EffectShaderResourceVariable* m_pNormalMap;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMap;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMap;
};

