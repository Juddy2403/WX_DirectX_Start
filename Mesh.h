#pragma once
#include "Effect.h"
#include "Texture.h"

struct Vertex {
	dae::Vector3 position;
	dae::Vector2 texCoord;
	dae::Vector3 color;
};

struct Vertex_Out {
	dae::Vector4 position;
	dae::Vector2 texCoord;
	dae::Vector3 color;
};

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice,const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices);
	~Mesh();
	void Render(ID3D11DeviceContext* m_pDeviceContext);
	void UpdateWorldViewProjMatrix(const float* pData);

	void SetDiffuseMap(Texture* pDiffuseTexture);

private:
	Effect* m_pEffect;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	uint32_t m_NumIndices;
};

