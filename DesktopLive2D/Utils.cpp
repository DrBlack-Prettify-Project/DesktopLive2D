#include "Utils.h"
#include <Windows.h>

void Utils::MbcToWchar(const char* src, size_t srcLength, wchar_t* dest, size_t destLength)
{
    if (srcLength < 1 || destLength < 2) return;

    memset(dest, 0, destLength);
    MultiByteToWideChar(CP_ACP, 0, src, static_cast<int>(srcLength), dest, static_cast<int>(destLength));
}
