//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "utf8conv.h"
#include "Utilities.h"

#include "LDtkLoader/Layer.hpp"
#include "LDtkLoader/Level.hpp"
#include "LDtkLoader/World.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
	m_window(nullptr),
	m_outputWidth(1600),
	m_outputHeight(800),
	m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
	m_world.loadFromFile("AutoLayers_1_basic.ldtk");
	const auto& all_levels = m_world.allLevels();
	const auto& level = all_levels[0];
	m_level = level.name;

	auto& layers = level.allLayers();
	auto it = layers.begin();
	while (it != layers.end())
	{
		if (it->getType() == ldtk::LayerType::Entities)
		{
			std::vector<EntityWrapper> wrapped_entities;
			for_each(it->allEntities().begin(), it->allEntities().end(), [&](const ldtk::Entity& e) { wrapped_entities.emplace_back(e, m_world.getDefaultCellSize()); });
			m_wrapped_entities_of_layers.insert(std::pair<std::string, std::vector<EntityWrapper>>(it->getName(), wrapped_entities));
		}
		++it;
	}
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_window = window;
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	CreateDevice();

	CreateResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// game logic
	for (auto& entry : m_wrapped_entities_of_layers)
	{
		for (auto& entity : entry.second)
		{
			entity.Move(elapsedTime);
		}
	}
}

// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	auto matrix = DirectX::XMMatrixIdentity();
	matrix *= DirectX::XMMatrixScaling(2, 2, 1);

	m_spriteBatch->Begin(
		SpriteSortMode_Deferred,
		m_states->NonPremultiplied(),
		m_states->PointClamp(),
		nullptr,
		nullptr,
		nullptr,
		matrix
	);

	const auto& level = m_world.getLevel(m_level);
	const auto& layers = level.allLayers();
	auto it = layers.rbegin();
	while (it != layers.rend())
	{
		if (it->hasTileset())
		{
			const auto& tileset = it->getTileset();
			auto texture = GetTexture(tileset.path);

			for (const auto& tile : it->allTiles())
			{
				// find that tile in the tileset
				// draw the tile

				RECT src;
				src.left = tile.texture_position.x;
				src.top = tile.texture_position.y;
				src.right = tile.texture_position.x + tileset.tile_size;
				src.bottom = tile.texture_position.y + tileset.tile_size;

				SpriteEffects se;
				if (!tile.flipX && !tile.flipY)
					se = SpriteEffects_None;
				else if (tile.flipX && !tile.flipY)
					se = SpriteEffects_FlipHorizontally;
				else if (!tile.flipX) // we don't have to check y
					se = SpriteEffects_FlipVertically;
				else
					se = SpriteEffects_FlipBoth;

				m_spriteBatch->Draw(
					texture.Get(),
					XMFLOAT2(static_cast<float>(tile.position.x), static_cast<float>(tile.position.y)),
					&src,
					Colors::White,
					0.0F,
					XMFLOAT2(0.0F, 0.0F),
					1.0F,
					se
				);
			}
		}
		else if (it->getType() == ldtk::LayerType::Entities)
		{
			for (auto& wrapped_entity : m_wrapped_entities_of_layers[it->getName()])
			{
				auto& entity = wrapped_entity.GetEntity();

				if (!entity.hasTile()) continue;

				auto texture = GetTexture(entity.getTileset().path);
				auto rect = entity.getTextureRect();
				RECT src;
				src.left = rect.x;
				src.top = rect.y;
				src.right = rect.x + rect.width;
				src.bottom = rect.y + rect.height;

				// minus width and height to subtract transform
				ldtk::Point<float> pos = TransformByPivot(entity.getPivot(), wrapped_entity.GetPos(), -entity.getSize().x, -entity.getSize().y);

				m_spriteBatch->Draw(
					texture.Get(),
					XMFLOAT2(pos.x, pos.y),
					&src,
					Colors::White,
					0.0F,
					XMFLOAT2(0.0F, 0.0F),
					XMFLOAT2(static_cast<float>(entity.getSize().x) / static_cast<float>(rect.width), static_cast<float>(entity.getSize().y) / static_cast<float>(rect.height))
				);
			}
		}

		it++;
	}

	m_spriteBatch->End();

	Present();
}

ComPtr<ID3D11ShaderResourceView> Game::GetTexture(std::string path)
{
	auto i = m_tileset_textures.find(path);
	if (i == m_tileset_textures.end())
		throw std::runtime_error("Layer specifies tileset that has not been loaded");
	return i->second;
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	// Clear the views.
	auto c = m_world.getLevel(m_level).bg_color;
	FLOAT f_c[4] = {
		static_cast<FLOAT>(c.r) / 255,
		static_cast<FLOAT>(c.g) / 255,
		static_cast<FLOAT>(c.b) / 255,
		static_cast<FLOAT>(c.a) / 255,
	};
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), f_c);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Set the viewport.
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
	m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(1, 0);

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		OnDeviceLost();
	}
	else
	{
		DX::ThrowIfFailed(hr);
	}
}

// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	CreateResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// Change to desired default window size (note minimum size is 320x200).
	width = static_cast<int>(m_world.getLevel(m_level).size.x * m_scale);
	height = static_cast<int>(m_world.getLevel(m_level).size.y * m_scale);
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		// TODO: Modify for supported Direct3D feature levels
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	DX::ThrowIfFailed(D3D11CreateDevice(
		nullptr,                            // specify nullptr to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		static_cast<UINT>(std::size(featureLevels)),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
		&m_featureLevel,                    // returns feature level of device created
		context.ReleaseAndGetAddressOf()    // returns the device immediate context
	));

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	DX::ThrowIfFailed(device.As(&m_d3dDevice));
	DX::ThrowIfFailed(context.As(&m_d3dContext));

	// Initialize device dependent objects here (independent of window size).

	m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());

	for (auto& ts : m_world.allTilesets())
	{
		auto m = m_tileset_textures.find(ts.path);

		// we haven't seen this tileset path yet
		if (m == m_tileset_textures.end())
		{
			ComPtr<ID3D11ShaderResourceView> texture;
			DX::ThrowIfFailed(CreateWICTextureFromFile(
				m_d3dDevice.Get(),
				Utf8ToUtf16(ts.path).c_str(),
				nullptr,
				texture.ReleaseAndGetAddressOf()
			));
			m_tileset_textures.insert(std::pair<std::string, ComPtr<ID3D11ShaderResourceView>>(ts.path, texture));
		}
	}
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_d3dContext->Flush();

	const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
	const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
	const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	constexpr UINT backBufferCount = 2;

	// If the swap chain already exists, resize it, otherwise create one.
	if (m_swapChain)
	{
		HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		auto hr = dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_swapChain.ReleaseAndGetAddressOf()
		);
		// invalid param, possibly the swap chain type isn't supported. Let's try another
		if (hr == 0x887A0001)
		{
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			hr = dxgiFactory->CreateSwapChainForHwnd(
				m_d3dDevice.Get(),
				m_window,
				&swapChainDesc,
				&fsSwapChainDesc,
				nullptr,
				m_swapChain.ReleaseAndGetAddressOf()
			);
		}
		DX::ThrowIfFailed(hr);

		// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
		DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	// Create a view interface on the rendertarget to use on bind.
	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

	// Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
	// Add Direct3D resource cleanup here.

	for (auto& t : m_tileset_textures)
	{
		t.second.Reset();
	}

	m_spriteBatch.reset();


	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();

	m_states.reset();

	m_spriteBatch.reset();

	CreateDevice();

	CreateResources();
}
