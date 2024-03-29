#include "pch.h"
#include "Renderer.h"
#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//const std::vector<Vertex> vertices
		//{

		//	//{{ 0.f, 3.f, 2.f}, {1.0f,0.0f,0.0f}}, //texcoord
		//	//{{ 3.f,-3.f, 2.f}, {0.0f,0.0f,1.0f}},
		//	//{{-3.f,-3.f, 2.f}, {0.0f,1.0f,0.0f}},
		//	Vertex{ Vector3{-3,3,-2}, Vector2{0,0}},
		//		Vertex{ Vector3{0,3,-2}, Vector2{0.5f,0}},
		//		Vertex{ Vector3{3,3,-2}, Vector2{1,0}},
		//		Vertex{ Vector3{-3,0,-2}, Vector2{0,0.5f}},
		//		Vertex{ Vector3{0,0,-2}, Vector2{0.5f,0.5f}},
		//		Vertex{ Vector3{3,0,-2}, Vector2{1,0.5f}},
		//		Vertex{ Vector3{-3,-3,-2}, Vector2{0,1}},
		//		Vertex{ Vector3{0,-3,-2}, Vector2{0.5f,1}},
		//		Vertex{ Vector3{3,-3,-2}, Vector2{1,1}}
		//};

		//const std::vector<uint32_t> indices{ 
		//			3,0,1,  1,4,3,  4,1,2,  //triangle list
		//			2,5,4,  6,3,4,  4,7,6,
		//			7,4,5,  5,8,7 
		//			/*3,0,4,1,5,2,			//trianlge strip
		//			2,6,
		//			6,3,7,4,8,5*/ };
		m_Camera.Initialize(45.f, Vector3{ 0.f,0.f,-50.f }, static_cast<float>(m_Width) / static_cast<float>(m_Height));

		//Parsing the obj file
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		if (!Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices))
			std::wcout << L"Mesh init failed!\n";
		m_pMesh = new Mesh{ m_pDevice,vertices,indices };

		if (!Utils::ParseOBJ("Resources/fireFX.obj", vertices, indices))
			std::wcout << L"Fire effect init failed!\n";
		m_pFireEffect = new Mesh{ m_pDevice,vertices,indices,false };

		//Loading texture maps
		m_pDiffuseTexture = new Texture{ "Resources/vehicle_diffuse.png",m_pDevice };
		m_pSpecularTexture = new Texture{ "Resources/vehicle_specular.png",m_pDevice };
		m_pGlossinessTexture = new Texture{ "Resources/vehicle_gloss.png",m_pDevice };
		m_pNormalTexture = new Texture{ "Resources/vehicle_normal.png",m_pDevice };
		m_pFireTexture = new Texture{ "Resources/fireFX_diffuse.png",m_pDevice };
		
		m_pFireEffect->SetDiffuseMap(m_pFireTexture);
		m_pMesh->SetDiffuseMap(m_pDiffuseTexture);
		m_pMesh->SetGlossinessMap(m_pGlossinessTexture);
		m_pMesh->SetNormalMap(m_pNormalTexture);
		m_pMesh->SetSpecularMap(m_pSpecularTexture);
	}

	Renderer::~Renderer()
	{
		if (m_pMesh) delete m_pMesh;
		if (m_pFireEffect) delete m_pFireEffect;

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice) m_pDevice->Release();
		if (m_pSwapChain)
		{
			m_pSwapChain->SetFullscreenState(false, nullptr);
			m_pSwapChain->Release();
		}
		if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
		if (m_pDepthStencilView) m_pDepthStencilView->Release();
		if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
		if (m_pRenderTargetView) m_pRenderTargetView->Release();

		//Releasing textures
		delete m_pDiffuseTexture;
		delete m_pGlossinessTexture;
		delete m_pSpecularTexture;
		delete m_pNormalTexture;
		delete m_pFireTexture;

	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
		m_pMesh->SetCameraPos(&m_Camera.origin.x);

		if(m_IsRotating)
		{
			const float degreesPerSec{ 45.f };
			m_YawRotation +=TO_RADIANS * (pTimer->GetElapsed() * degreesPerSec);
			m_pMesh->RotateMesh(Vector3{ 0.f,m_YawRotation,0.f });
			
		}
		const Matrix viewProjMatrix = m_pMesh->GetWorldMatrix() * m_Camera.viewMatrix * m_Camera.projectionMatrix;
		m_pMesh->UpdateWorldViewProjMatrix(&viewProjMatrix.data[0].x);
		m_pFireEffect->UpdateWorldViewProjMatrix(&viewProjMatrix.data[0].x);

	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//1. Clear RTV & DSV
		constexpr float color[4] = { 0.39f,0.59f,0.93f,1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. Set pipeline + invoke draw calls (=render)
		m_pMesh->Render(m_pDeviceContext);
		if(m_IsFireMeshActive)	m_pFireEffect->Render(m_pDeviceContext);
		//3. Present backbuffer (SWAP)
		m_pSwapChain->Present(0, 0);

	}

	void Renderer::ChangeSamplerState()
	{
		m_pMesh->ChangeSamplerState();
		m_pFireEffect->ChangeSamplerState();
	}

	void Renderer::ToggleRotation()
	{
		m_IsRotating = !m_IsRotating;
	}

	void Renderer::ToggleNormals()
	{
		m_pMesh->ToggleNormals();
	}

	void Renderer::ToggleFireMesh()
	{
		m_IsFireMeshActive = !m_IsFireMeshActive;
		std::wcout << L"Is fire mesh active: " << std::boolalpha << m_IsFireMeshActive<<"\n";
	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. Creating Device & DeviceContext
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1,
			D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result))
			return result;

		//Create DXGI Factory (for the swap chain)
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//2. Create SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get the handle (HWND) from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		// Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//4. Create RenderTarget (RT) & RenderTargetView (RTV)

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		//5. Bind RTV & DSV to Output Merger Stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		//6. Set Viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return S_OK;
	}
}
