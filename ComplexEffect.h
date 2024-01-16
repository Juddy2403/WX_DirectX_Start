#pragma once
#include "Texture.h"
#include "Effect.h"

class ComplexEffect final : public Effect
{
public:
	ComplexEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~ComplexEffect() override;
	
	void SetWorldMatrix(const float* pData);
	void SetCameraPos(const float* pData);

	void SetGlossinessMap(Texture* pGlossinessTexture);
	void SetSpecularMap(Texture* pSpecularTexture);
	void SetNormalMap(Texture* pNormalTexture);
private:
	//Extra variables needed for Phong calculus
	ID3DX11EffectMatrixVariable* m_pWorldMatrix;
	ID3DX11EffectVectorVariable* m_pCameraPos;

	//2DTexture maps
	ID3DX11EffectShaderResourceVariable* m_pNormalMap;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMap;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMap;
};

