#pragma once
#include <CubismFramework.hpp>

class Director
{
public:
	static void StartTimer();

	static Csm::csmFloat32 GetDeltaTime();

	static void UpdateTime();
};

