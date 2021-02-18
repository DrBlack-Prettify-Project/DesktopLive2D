#pragma once
#include <Windows.h>
#include <CubismFramework.hpp>
#include <boost/filesystem.hpp>

class Config
{
private:
	nlohmann::json defaultConfig;

	static boost::filesystem::path getConfigFilePath();
public:
	Config();

	Csm::csmInt32 Live2DWidth = 190;
	Csm::csmInt32 Live2DHeight = 100;

	BOOL loadConfig();

	BOOL saveConfig();

	BOOL resetConfig();
};

