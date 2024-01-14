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
	void SetDiffuseMap(Texture* pDiffuseTexture);
private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pWorldViewProjMatrix;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;
};

