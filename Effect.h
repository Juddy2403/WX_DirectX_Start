#pragma once
#include "Texture.h"

class Effect 
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~Effect();
	
	virtual ID3DX11Effect* GetEffect();
	virtual ID3DX11EffectTechnique* GetTechnique();
	virtual void SetWorldViewProjMatrix(const float* pData);
	virtual void SetDiffuseMap(Texture* pDiffuseTexture);
protected:
	ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pWorldViewProjMatrix;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;
};