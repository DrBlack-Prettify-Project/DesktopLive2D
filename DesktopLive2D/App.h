#pragma once
#include <Windows.h>
#include <d3d11.h>

#include "Config.h"
#include "TextureManager.h"

class App
{
private:
	static App* _instance;

	HINSTANCE hInstance;
	HWND wHwnd;

	Csm::CubismFramework::Option _cubismOption;

	TextureManager* textureManager;

	DXGI_SWAP_CHAIN_DESC presentParameters;
	ID3D11Device* device;
	IDXGISwapChain* swapChain;
	ID3D11DeviceContext* deviceContext;

	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthState;

	ID3D11RasterizerState* rasterizer;
	ID3D11SamplerState* samplerState;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11BlendState* blendState;
	ID3D11InputLayout* vertexFormat;

	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	BOOL CreateRenderTarget(UINT width, UINT height);
public:
	static App* GetInstance();

	Config config;

	App(HINSTANCE instance);

	BOOL Initialize();

	BOOL CreateShader();

	void InitalizeCubism();

	ID3D11Device* GetD3dDevice();

	ID3D11DeviceContext* GetD3dContext();

	void Run();

	void Release();

	void ReleaseShader();
};

