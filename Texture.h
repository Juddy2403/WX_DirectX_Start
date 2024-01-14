#pragma once
class Texture
{
private:
	ID3D11Texture2D* m_pResource;
	ID3D11ShaderResourceView* m_pSRV;
public:
	Texture(const char* path, ID3D11Device* pDevice);
	~Texture();
	ID3D11ShaderResourceView* GetTexture() const;

};

