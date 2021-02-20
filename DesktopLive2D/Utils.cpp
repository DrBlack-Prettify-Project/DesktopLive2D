#include "Utils.h"

#include <string>
#include <fstream>
#include <Windows.h>
#include <CubismFramework.hpp>

Csm::csmByte* Utils::LoadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize)
{
    const char* path = filePath.c_str();

    int size = 0;
    struct stat statBuf;
    if (stat(path, &statBuf) == 0)
    {
        size = statBuf.st_size;
    }

    std::fstream file;
    char* buf = new char[size];

    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        OutputDebugString(L"Cannot read file\n");
        return NULL;
    }
    file.read(buf, size);
    file.close();

    *outSize = size;
    return reinterpret_cast<Csm::csmByte*>(buf);
}

void Utils::ReleaseBytes(Csm::csmByte* byteData)
{
    delete[] byteData;
}

void Utils::MbcToWchar(const char* src, size_t srcLength, wchar_t* dest, size_t destLength)
{
    if (srcLength < 1 || destLength < 2) return;

    memset(dest, 0, destLength);
    MultiByteToWideChar(CP_ACP, 0, src, static_cast<int>(srcLength), dest, static_cast<int>(destLength));
}

void Utils::CoordinateFullScreenToWindow(float clientWidth, float clientHeight, float fullScreenX, float fullScreenY, float& retWindowX, float& retWindowY)
{
    retWindowX = retWindowY = 0.0f;

    const float width = static_cast<float>(clientWidth);
    const float height = static_cast<float>(clientHeight);

    if (width == 0.0f || height == 0.0f) return;

    retWindowX = (fullScreenX + width) * 0.5f;
    retWindowY = (-fullScreenY + height) * 0.5f;
}

void Utils::CoordinateWindowToFullScreen(float clientWidth, float clientHeight, float windowX, float windowY, float& retFullScreenX, float& retFullScreenY)
{
    retFullScreenX = retFullScreenY = 0.0f;

    const float width = static_cast<float>(clientWidth);
    const float height = static_cast<float>(clientHeight);

    if (width == 0.0f || height == 0.0f) return;

    retFullScreenX = 2.0f * windowX - width;
    retFullScreenY = (2.0f * windowY - height) * -1.0f;
}