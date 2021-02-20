#pragma once
#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <CubismFramework.hpp>

class Sprite
{
public:
	struct Rect {
	public:
		float left;
		float right;
		float up;
		float down;
	};

	struct SpriteVertex {
		float x, y;
		float u, v;
	};

	Sprite();

	Sprite(float x, float y, float width, float height, Csm::csmUint64 textureId);

	~Sprite();

	Csm::csmUint64 GetTextureId() { return textureId; }

	void Render(int width, int height) const;

	void RenderImmidiate(int width, int height, ID3D11ShaderResourceView* resourceView) const;

	bool IsHit(float pointX, float pointY) const;

	void ResetRect(float x, float y, float width, float height);

	void SetColor(float r, float g, float b, float a);

	static const int VERTEX_NUM = 4;
	static const int INDEX_NUM = 6;
private:

	Csm::csmUint64 textureId;
	Rect rect;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* constantBuffer;

	DirectX::XMFLOAT4   color;
};

