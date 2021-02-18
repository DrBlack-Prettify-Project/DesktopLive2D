#pragma once
#include <string>

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <Type/CubismBasicType.hpp>
#include <Type/csmVector.hpp>

class TextureManager
{
public:
	struct TextureInfo {
		Csm::csmUint64 id;
		int width;
		int height;
		std::string fileName;
	};

	TextureManager();

	~TextureManager();

	inline unsigned int Premultiply(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	{
		return static_cast<unsigned>(\
			(red * (alpha + 1) >> 8) | \
			((green * (alpha + 1) >> 8) << 8) | \
			((blue * (alpha + 1) >> 8) << 16) | \
			(((alpha)) << 24)   \
			);
	}

	TextureInfo* CreateTextureFromPngFile(std::string fileName, bool isPreMult, UINT maxSize = 0);

	void ReleaseTextures();

	void ReleaseTexture(Csm::csmUint64 textureId);

	void ReleaseTexture(std::string fileName);

	bool GetTexture(Csm::csmUint64 textureId, ID3D11ShaderResourceView*& retTexture) const;

	TextureInfo* GetTextureInfoByName(std::string& fileName) const;

	TextureInfo* GetTextureInfoById(Csm::csmUint64 textureId) const;
private:
	Csm::csmVector<ID3D11Resource*>             _textures;
	Csm::csmVector<ID3D11ShaderResourceView*>   _textureView;
	Csm::csmVector<TextureInfo*>                _texturesInfo;

	Csm::csmUint64   _sequenceId;
};

