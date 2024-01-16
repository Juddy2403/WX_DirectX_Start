#include "pch.h"
#include "Mesh.h"
#include <cassert>

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) :
	m_pEffect{ new Effect{pDevice, L"Resources/PosCol3D.fx"} },
	m_pInputLayout{ nullptr },
	m_NumIndices{ static_cast<uint32_t>(indices.size()) },
	m_WorldMatrix{},
	m_Translation{},
	m_Scale{1,1,1},
	m_RotationAngle{}
{
	// Create vertex layout
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "COLOR";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	assert(result == S_OK);

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	assert(result == S_OK);

	//Create index buffer
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	assert(result == S_OK);

}

Mesh::~Mesh()
{
	if (m_pEffect) delete m_pEffect;
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pIndexBuffer) m_pIndexBuffer->Release();

}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	//1. Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set input layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set IndexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	if (UINT(m_SamplerState) < techDesc.Passes)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(UINT(m_SamplerState))->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::UpdateWorldViewProjMatrix(const float* pData)
{
	m_pEffect->SetWorldViewProjMatrix(pData);
}

void Mesh::SetCameraPos(const float* pData)
{
	m_pEffect->SetCameraPos(pData);
}

void Mesh::UpdateWorldMatrix()
{
	m_pEffect->SetWorldMatrix(&m_WorldMatrix.data[0].x);
}


void Mesh::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pEffect) m_pEffect->SetDiffuseMap(pDiffuseTexture);
	else std::wcout << L"Can't set diffuse map! m_pEffect is nullptr\n";
}

void Mesh::SetGlossinessMap(Texture* pGlossinessTexture)
{
	if (m_pEffect) m_pEffect->SetGlossinessMap(pGlossinessTexture);
	else std::wcout << L"Can't set glossiness map! m_pEffect is nullptr\n";
}

void Mesh::SetSpecularMap(Texture* pSpecularTexture)
{
	if (m_pEffect) m_pEffect->SetSpecularMap(pSpecularTexture);
	else std::wcout << L"Can't set specular map! m_pEffect is nullptr\n";
}

void Mesh::SetNormalMap(Texture* pNormalTexture)
{
	if (m_pEffect) m_pEffect->SetNormalMap(pNormalTexture);
	else std::wcout << L"Can't set normal map! m_pEffect is nullptr\n";
}

void Mesh::ChangeSamplerState()
{
	m_SamplerState = SamplerState((static_cast<int>(m_SamplerState) + 1) % 3);
	std::wcout<<L"Sampler state changed: " << int(m_SamplerState) << "\n";
}

dae::Matrix Mesh::GetWorldMatrix()
{
	return m_WorldMatrix;
}

void Mesh::RotateMesh(const dae::Vector3& rotation)
{
	m_WorldMatrix = dae::Matrix::CreateRotation(rotation);
	UpdateWorldMatrix();
}
