#include "Sprite.h"
#include "App.h"
#include "Utils.h"

#include <Rendering/D3D11/CubismType_D3D11.hpp>

Sprite::Sprite() : rect(), vertexBuffer(NULL), indexBuffer(NULL), constantBuffer(NULL) {
	color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

Sprite::Sprite(float x, float y, float width, float height, Csm::csmUint64 textureId) : rect(), vertexBuffer(NULL), indexBuffer(NULL), constantBuffer(NULL) {
	color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	rect.left = (x - width * 0.5f);
	rect.right = (x + width * 0.5f);
	rect.up = (y + height * 0.5f);
	rect.down = (y - height * 0.5f);
	Sprite::textureId = textureId;

	ID3D11Device* device = App::GetInstance()->GetD3dDevice();

	if (!device) { return; }

    HRESULT hr = S_OK;

    if (!vertexBuffer)
    {
        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(SpriteVertex) * VERTEX_NUM;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        hr = device->CreateBuffer(&bufferDesc, NULL, &vertexBuffer);
        if (FAILED(hr))
        {
            OutputDebugString(L"Cannot allocate vertex data in Sprite\n");
            return;
        }
    }

    if (!indexBuffer)
    {
        WORD idx[INDEX_NUM] = {
            0, 1, 2,
            1, 3, 2,
        };

        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(WORD) * INDEX_NUM;
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subResourceData;
        memset(&subResourceData, 0, sizeof(subResourceData));
        subResourceData.pSysMem = idx;
        subResourceData.SysMemPitch = 0;
        subResourceData.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&bufferDesc, &subResourceData, &indexBuffer);
        if (FAILED(hr))
        {
            OutputDebugString(L"Cannot allocate index data in Sprite\n");
            return;
        }
    }

    if (!constantBuffer)
    {
        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(Live2D::Cubism::Framework::CubismConstantBufferD3D11);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        hr = device->CreateBuffer(&bufferDesc, NULL, &constantBuffer);
    }
}

Sprite::~Sprite()
{
    App::GetInstance()->GetTextureManager()->ReleaseTexture(textureId);

    if (constantBuffer)
    {
        constantBuffer->Release();
        constantBuffer = NULL;
    }
    if (indexBuffer)
    {
        indexBuffer->Release();
        indexBuffer = NULL;
    }
    if (vertexBuffer)
    {
        vertexBuffer->Release();
        vertexBuffer = NULL;
    }
}

void Sprite::Render(int width, int height) const
{
    if (width == 0 || height == 0)
    {
        return;
    }

    App* app = App::GetInstance();
    ID3D11DeviceContext* renderContext = app->GetD3dContext();

    SpriteVertex vtx[VERTEX_NUM] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 1.0f, 1.0f },
    };

    vtx[0].x = (rect.left - width * 0.5f) / (width * 0.5f); vtx[0].y = (rect.down - height * 0.5f) / (height * 0.5f);
    vtx[1].x = (rect.right - width * 0.5f) / (width * 0.5f); vtx[1].y = (rect.down - height * 0.5f) / (height * 0.5f);
    vtx[2].x = (rect.left - width * 0.5f) / (width * 0.5f); vtx[2].y = (rect.up - height * 0.5f) / (height * 0.5f);
    vtx[3].x = (rect.right - width * 0.5f) / (width * 0.5f); vtx[3].y = (rect.up - height * 0.5f) / (height * 0.5f);

    if (vertexBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE subRes;
        if (SUCCEEDED(renderContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
        {
            memcpy(subRes.pData, vtx, sizeof(SpriteVertex) * VERTEX_NUM);
            renderContext->Unmap(vertexBuffer, 0);
        }
    }

    if (constantBuffer)
    {
        Live2D::Cubism::Framework::CubismConstantBufferD3D11 cb;
        memset(&cb, 0, sizeof(cb));
        cb.baseColor = color;
        DirectX::XMMATRIX proj = DirectX::XMMatrixIdentity();
        XMStoreFloat4x4(&cb.projectMatrix, XMMatrixTranspose(proj));
        renderContext->UpdateSubresource(constantBuffer, 0, NULL, &cb, 0, 0);

        renderContext->VSSetConstantBuffers(0, 1, &constantBuffer);
        renderContext->PSSetConstantBuffers(0, 1, &constantBuffer);
    }

    {
        UINT strides = sizeof(Sprite::SpriteVertex);
        UINT offsets = 0;

        renderContext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
        renderContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
        renderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        app->SetupShader();

        ID3D11ShaderResourceView* textureView = NULL;
        if (App::GetInstance()->GetTextureManager()->GetTexture(textureId, textureView))
        {
            renderContext->PSSetShaderResources(0, 1, &textureView);
        }

        renderContext->DrawIndexed(INDEX_NUM, 0, 0);
    }
}

void Sprite::RenderImmidiate(int width, int height, ID3D11ShaderResourceView* resourceView) const
{
    if (!resourceView) return;

    if (width == 0 || height == 0)
    {
        return;
    }

    App* app = App::GetInstance();
    ID3D11DeviceContext* renderContext = app->GetD3dContext();

    Sprite::SpriteVertex vtx[Sprite::VERTEX_NUM] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 1.0f, 1.0f },
    };

    vtx[0].x = (rect.left - width * 0.5f) / (width * 0.5f); vtx[0].y = (rect.down - height * 0.5f) / (height * 0.5f);
    vtx[1].x = (rect.right - width * 0.5f) / (width * 0.5f); vtx[1].y = (rect.down - height * 0.5f) / (height * 0.5f);
    vtx[2].x = (rect.left - width * 0.5f) / (width * 0.5f); vtx[2].y = (rect.up - height * 0.5f) / (height * 0.5f);
    vtx[3].x = (rect.right - width * 0.5f) / (width * 0.5f); vtx[3].y = (rect.up - height * 0.5f) / (height * 0.5f);

    if (vertexBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE subRes;
        if (SUCCEEDED(renderContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
        {
            memcpy(subRes.pData, vtx, sizeof(Sprite::SpriteVertex) * VERTEX_NUM);
            renderContext->Unmap(vertexBuffer, 0);
        }
    }

    if (constantBuffer)
    {
        Live2D::Cubism::Framework::CubismConstantBufferD3D11 cb;
        memset(&cb, 0, sizeof(cb));
        cb.baseColor = color;
        DirectX::XMMATRIX proj = DirectX::XMMatrixIdentity();
        XMStoreFloat4x4(&cb.projectMatrix, XMMatrixTranspose(proj));
        renderContext->UpdateSubresource(constantBuffer, 0, NULL, &cb, 0, 0);

        renderContext->VSSetConstantBuffers(0, 1, &constantBuffer);
        renderContext->PSSetConstantBuffers(0, 1, &constantBuffer);
    }

    {
        UINT strides = sizeof(SpriteVertex);
        UINT offsets = 0;

        renderContext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
        renderContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
        renderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        app->SetupShader();

        {
            renderContext->PSSetShaderResources(0, 1, &resourceView);
        }

        renderContext->DrawIndexed(INDEX_NUM, 0, 0);
    }
}

bool Sprite::IsHit(float pointX, float pointY) const
{
    float coordX = 0.0f, coordY = 0.0f;
    int clientWidth = 0, clientHeight = 0;
    App::GetClientSize(clientWidth, clientHeight);
    Utils::CoordinateWindowToFullScreen(static_cast<float>(clientWidth), static_cast<float>(clientHeight), pointX, pointY, coordX, coordY);

    if (clientWidth == 0 || clientHeight == 0)
    {
        return false;
    }

    coordX = (clientWidth + coordX) / (2.0f * clientWidth) * clientWidth;
    coordY = (clientHeight + coordY) / (2.0f * clientHeight) * clientHeight;

    return (coordX >= rect.left && coordX <= rect.right && coordY <= rect.up && coordY >= rect.down);
}

void Sprite::ResetRect(float x, float y, float width, float height)
{
    rect.left = (x - width * 0.5f);
    rect.right = (x + width * 0.5f);
    rect.up = (y + height * 0.5f);
    rect.down = (y - height * 0.5f);
}

void Sprite::SetColor(float r, float g, float b, float a)
{
    color = DirectX::XMFLOAT4(r, g, b, a);
}
