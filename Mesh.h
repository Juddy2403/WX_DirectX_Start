#pragma once
#include "ComplexEffect.h"
#include "Texture.h"

struct Vertex {
	dae::Vector3 position;
	dae::Vector2 texCoord;
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
	dae::ColorRGB color{ dae::colors::White };
};

//struct Vertex_Out {
//	dae::Vector4 position;
//	dae::Vector2 texCoord;
//	dae::Vector3 color;
//};

enum class SamplerState {
	point,
	linear,
	anisotropic
};

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice,const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices, bool isComplexMesh = true);
	~Mesh();
	void Render(ID3D11DeviceContext* m_pDeviceContext);
	void UpdateWorldViewProjMatrix(const float* pData);
	void SetCameraPos(const float* pData);

	void SetDiffuseMap(Texture* pDiffuseTexture);
	void SetGlossinessMap(Texture* pGlossinessTexture);
	void SetSpecularMap(Texture* pSpecularTexture);
	void SetNormalMap(Texture* pNormalTexture);
	
	void ChangeSamplerState();
	dae::Matrix GetWorldMatrix();
	void RotateMesh(const dae::Vector3& rotation);
	void ToggleNormals();
private:
	void UpdateWorldMatrix();

	Effect* m_pEffect;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	uint32_t m_NumIndices;
	dae::Matrix m_WorldMatrix;
	dae::Vector3 m_RotationAngle;
	dae::Vector3 m_Translation;
	dae::Vector3 m_Scale;
	SamplerState m_SamplerState{ SamplerState::point };
};

