#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		void ChangeSamplerState();
		void ToggleRotation();
		void ToggleNormals();
		void ToggleFireMesh();
	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };
		bool m_IsFireMeshActive{ true };
		//DIRECTX
		HRESULT InitializeDirectX();
		//...
		ID3D11Device* m_pDevice{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11DeviceContext* m_pDeviceContext{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		Camera m_Camera;
		Mesh* m_pMesh{};
		Mesh* m_pFireEffect{};
		Texture* m_pDiffuseTexture;
		Texture* m_pGlossinessTexture;
		Texture* m_pSpecularTexture;
		Texture* m_pNormalTexture;
		Texture* m_pFireTexture;
		bool m_IsRotating{ true };
		float m_YawRotation{};
	};
}
