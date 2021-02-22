#include "Director.h"

#include <CubismFramework.hpp>
#include <Windows.h>

namespace {
    LARGE_INTEGER s_frequency;
    LARGE_INTEGER s_lastFrame;
    double s_deltaTime = 0.0;
}

void Director::StartTimer()
{
    QueryPerformanceFrequency(&s_frequency);
}

Csm::csmFloat32  Director::GetDeltaTime()
{
    return static_cast<Csm::csmFloat32>(s_deltaTime);
}

void Director::UpdateTime()
{
    if (s_frequency.QuadPart == 0)
    {
        StartTimer();
        QueryPerformanceCounter(&s_lastFrame);
        s_deltaTime = 0.0f;
        return;
    }

    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);

    const LONGLONG BASIS = 1000000;
    LONGLONG dwTime = ((current.QuadPart - s_lastFrame.QuadPart) * (LONGLONG)BASIS / s_frequency.QuadPart);
    s_deltaTime = (double)dwTime / (double)(BASIS);
    s_lastFrame = current;
}