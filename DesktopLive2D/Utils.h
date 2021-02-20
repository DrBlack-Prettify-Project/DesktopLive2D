#pragma once

#include <CubismFramework.hpp>

class Utils
{
public:
	static Csm::csmByte* LoadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize);

	static void ReleaseBytes(Csm::csmByte* byteData);

	static void CoordinateFullScreenToWindow(float clientWidth, float clientHeight, float fullScreenX, float fullScreenY, float& retWindowX, float& retWindowY);

	static void CoordinateWindowToFullScreen(float clientWidth, float clientHeight, float windowX, float windowY, float& retFullScreenX, float& retFullScreenY);

	static void MbcToWchar(const char* src, size_t srcLength, wchar_t* dest, size_t destLength);
};

