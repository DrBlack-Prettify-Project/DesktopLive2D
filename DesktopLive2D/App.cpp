#include "App.h"
#include <d3d11.h>
#include <d3dcompiler.h>

#include <CubismFramework.hpp>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include "Allocator.h"

LRESULT CALLBACK App::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    OutputDebugString(L"Msg");
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL App::CreateRenderTarget(UINT width, UINT height) {
    if (!swapChain || !device) {
        return FALSE;
    }

    HRESULT hr;
    ID3D11Texture2D* backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr)) {
        OutputDebugString(L"Failed to get swap chain buffer\n");
        return FALSE;
    }
    hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        OutputDebugString(L"Failed to create render target view\n");
        return FALSE;
    }

    D3D11_TEXTURE2D_DESC depthDesc;
    memset(&depthDesc, 0, sizeof(depthDesc));
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;
    hr = device->CreateTexture2D(&depthDesc, NULL, &depthTexture);
    if (FAILED(hr)) {
        OutputDebugString(L"Failed to create depth target\n");
        return FALSE;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
    memset(&depthViewDesc, 0, sizeof(depthViewDesc));
    depthViewDesc.Format = depthDesc.Format;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthViewDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(depthTexture, &depthViewDesc, &depthStencilView);
    if (FAILED(hr)) {
        OutputDebugString(L"Failed to create depth target view\n");
        return FALSE;
    }

    return TRUE;
}

App* App::GetInstance()
{
    return _instance;
}

App::App(HINSTANCE instance)
{
    hInstance = instance;
    wHwnd = NULL;
    _instance = this;
}

BOOL App::Initialize()
{
    config = Config();

    BOOL configResult = config.loadConfig();

    if (!configResult) {
        OutputDebugString(L"Cannot load config\n");
        return FALSE;
    }

    WNDCLASSEX wndClass;
    memset(&wndClass, 0, sizeof(wndClass));
    wndClass.cbSize = sizeof(wndClass);
    wndClass.lpfnWndProc = (WNDPROC)MsgProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = L"DesktopLive2D";

    int regClass = RegisterClassEx(&wndClass);
    if (!regClass) {
        OutputDebugString(L"Cannot register window class\n");
        return FALSE;
    }

    wHwnd = CreateWindowEx(WS_EX_TOPMOST, wndClass.lpszClassName, L"Desktop Live2D", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    
    if (!wHwnd) {
        OutputDebugString(L"Cannot create Live2D window\n");
        return FALSE;
    }

    ShowWindow(wHwnd, SW_SHOWDEFAULT);
    UpdateWindow(wHwnd);

    memset(&presentParameters, 0, sizeof(presentParameters));
    presentParameters.BufferCount = 1;
    presentParameters.BufferDesc.Width = config.Live2DWidth;
    presentParameters.BufferDesc.Height = config.Live2DHeight;
    presentParameters.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    presentParameters.BufferDesc.RefreshRate.Numerator = 60;
    presentParameters.BufferDesc.RefreshRate.Denominator = 1;
    presentParameters.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    presentParameters.SampleDesc.Count = 1;
    presentParameters.SampleDesc.Quality = 0;
    presentParameters.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    presentParameters.Flags = 0;
    presentParameters.Windowed = TRUE;
    presentParameters.OutputWindow = wHwnd;

    D3D_FEATURE_LEVEL level;
    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
#ifdef DEBUG
        D3D11_CREATE_DEVICE_DEBUG,
#else
        0,
#endif
        NULL,
        0,
        D3D11_SDK_VERSION,
        &presentParameters,
        &swapChain,
        &device,
        &level,
        &deviceContext);

    if (FAILED(result)) {
        OutputDebugString(L"Failed to create Direct3D device\n");
        return FALSE;
    }

    CreateRenderTarget(config.Live2DWidth, config.Live2DHeight);

    D3D11_DEPTH_STENCIL_DESC depthDesc;
    memset(&depthDesc, 0, sizeof(depthDesc));
    depthDesc.DepthEnable = false;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDesc.StencilEnable = false;
    result = device->CreateDepthStencilState(&depthDesc, &depthState);
    if (FAILED(result)) {
        OutputDebugString(L"Failed to create depth\n");
        return FALSE;
    }

    D3D11_RASTERIZER_DESC rasterDesc;
    memset(&rasterDesc, 0, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK; // ÑYÃæ¤òÇÐ¤ë
    rasterDesc.FrontCounterClockwise = TRUE; // CCW¤ò±íÃæ¤Ë¤¹¤ë
    rasterDesc.DepthClipEnable = FALSE;
    rasterDesc.MultisampleEnable = FALSE;
    rasterDesc.DepthBiasClamp = 0;
    rasterDesc.SlopeScaledDepthBias = 0;
    result = device->CreateRasterizerState(&rasterDesc, &rasterizer);
    if (FAILED(result)) {
        OutputDebugString(L"Failed to create rasterizer\n");
        return FALSE;
    }

    D3D11_SAMPLER_DESC samplerDesc;
    memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = device->CreateSamplerState(&samplerDesc, &samplerState);
    if (FAILED(result)) {
        OutputDebugString(L"Failed to create sampler");
        return FALSE;
    }

    CreateShader();

    textureManager = new TextureManager();

    view->Initialize();

    InitializeCubism();

    return TRUE;
}

BOOL App::CreateShader()
{
    ReleaseShader();

    static const Live2D::Cubism::Framework::csmChar* SpriteShaderEffectSrc =
        "cbuffer ConstantBuffer {"\
        "float4x4 projectMatrix;"\
        "float4x4 clipMatrix;"\
        "float4 baseColor;"\
        "float4 channelFlag;"\
        "}"\
        \
        "Texture2D mainTexture : register(t0);"\
        "SamplerState mainSampler : register(s0);"\
        "struct VS_IN {"\
        "float2 pos : POSITION0;"\
        "float2 uv : TEXCOORD0;"\
        "};"\
        "struct VS_OUT {"\
        "float4 Position : SV_POSITION;"\
        "float2 uv : TEXCOORD0;"\
        "float4 clipPosition : TEXCOORD1;"\
        "};"\
        \
        "/* Vertex Shader */"\
        "/* normal */"\
        "VS_OUT VertNormal(VS_IN In) {"\
        "VS_OUT Out = (VS_OUT)0;"\
        "Out.Position = mul(float4(In.pos, 0.0f, 1.0f), projectMatrix);"\
        "Out.uv.x = In.uv.x;"\
        "Out.uv.y = 1.0 - +In.uv.y;"\
        "return Out;"\
        "}"\
        \
        "/* Pixel Shader */"\
        "/* normal */"\
        "float4 PixelNormal(VS_OUT In) : SV_Target {"\
        "float4 color = mainTexture.Sample(mainSampler, In.uv) * baseColor;"\
        "return color;"\
        "}";

    ID3DBlob* vertexError = NULL;
    ID3DBlob* pixelError = NULL;

    ID3DBlob* vertexBlob = NULL;
    ID3DBlob* pixelBlob = NULL;

    HRESULT hr = S_OK;
    do
    {
        UINT compileFlag = 0;

        hr = D3DCompile(
            SpriteShaderEffectSrc,
            strlen(SpriteShaderEffectSrc),
            NULL,
            NULL,
            NULL,
            "VertNormal",
            "vs_4_0",
            compileFlag,
            0,
            &vertexBlob,
            &vertexError);
        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to compile vertex shader\n");
            break;
        }
        hr = device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);
        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to create vertex shader\n");
            break;
        }

        hr = D3DCompile(
            SpriteShaderEffectSrc,
            strlen(SpriteShaderEffectSrc),
            NULL,
            NULL,
            NULL,
            "PixelNormal",
            "ps_4_0",
            compileFlag,
            0,
            &pixelBlob,
            &pixelError);
        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to compile pixel shader\n");
            break;
        }

        hr = device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);
        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to create pixel shader\n");
            break;
        }

        D3D11_INPUT_ELEMENT_DESC elems[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        hr = device->CreateInputLayout(elems, ARRAYSIZE(elems), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexFormat);

        if (FAILED(hr))
        {
            OutputDebugString(L"Failed to create vertex declaration\n");
            break;
        }

    } while (0);

    if (pixelError)
    {
        pixelError->Release();
        pixelError = NULL;
    }
    if (vertexError)
    {
        vertexError->Release();
        vertexError = NULL;
    }

    if (pixelBlob)
    {
        pixelBlob->Release();
        pixelBlob = NULL;
    }
    if (vertexBlob)
    {
        vertexBlob->Release();
        vertexBlob = NULL;
    }

    if (FAILED(hr))
    {
        return false;
    }

    D3D11_BLEND_DESC blendDesc;
    memset(&blendDesc, 0, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&blendDesc, &blendState);
    return TRUE;
}

void App::SetupShader()
{
    if (device == NULL || vertexFormat == NULL || vertexShader == NULL || pixelShader == NULL)
    {
        return;
    }

    int windowWidth, windowHeight;
    GetClientSize(windowWidth, windowHeight);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    deviceContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    deviceContext->IASetInputLayout(vertexFormat);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->IASetInputLayout(vertexFormat);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<FLOAT>(windowWidth);
    viewport.Height = static_cast<FLOAT>(windowHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &viewport);
    deviceContext->RSSetState(rasterizer);

    deviceContext->VSSetShader(vertexShader, NULL, 0);

    deviceContext->PSSetShader(pixelShader, NULL, 0);
    deviceContext->PSSetSamplers(0, 1, &samplerState);
}

void App::InitializeCubism()
{
    Allocator _cubismAllocator;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    Csm::CubismFramework::Initialize();

    Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D11::InitializeConstantSettings(1, device);

    view->InitalizeSprite();
}

ID3D11Device* App::GetD3dDevice()
{
    return device;
}

ID3D11DeviceContext* App::GetD3dContext()
{
    return deviceContext;
}

TextureManager* App::GetTextureManager()
{
    return textureManager;
}

void App::GetWindowRect(RECT& rect)
{
    if (!_instance) {
        return;
    }

    GetClientRect(_instance->wHwnd, &rect);
}

void App::GetClientSize(int& rWidth, int& rHeight)
{
    if (!_instance)
    {
        return;
    }

    RECT clientRect;
    GetClientRect(_instance->wHwnd, &clientRect);

    rWidth = (clientRect.right - clientRect.left);
    rHeight = (clientRect.bottom - clientRect.top);
}

void App::Run()
{
    MSG msg;
    do {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {

        }
    } while (msg.message != WM_QUIT);
}

void App::ReleaseShader() {
    if (blendState)
    {
        blendState->Release();
        blendState = NULL;
    }
    if (vertexFormat)
    {
        vertexFormat->Release();
        vertexFormat = NULL;
    }
    if (pixelShader)
    {
        pixelShader->Release();
        pixelShader = NULL;
    }
    if (vertexShader)
    {
        vertexShader->Release();
        vertexShader = NULL;
    }
}

void App::Release()
{
    view->ReleaseSprite();

    Live2DManager::ReleaseInstance();

    ReleaseShader();

    delete view;
    delete textureManager;
    view = NULL;
    textureManager = NULL;

    if (samplerState)
    {
        samplerState->Release();
        samplerState = NULL;
    }
    if (rasterizer)
    {
        rasterizer->Release();
        rasterizer = NULL;
    }

    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = NULL;
    }
    if (depthState)
    {
        depthState->Release();
        depthState = NULL;
    }
    if (depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = NULL;
    }
    if (depthTexture)
    {
        depthTexture->Release();
        depthTexture = NULL;
    }

    if (swapChain)
    {
        swapChain->Release();
        swapChain = NULL;
    }
    if (deviceContext)
    {
        deviceContext->Release();
        deviceContext = NULL;
    }

    if (device)
    {
        device->Release();
        device = NULL;
    }
}
