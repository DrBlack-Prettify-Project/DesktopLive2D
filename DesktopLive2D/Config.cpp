#include "Config.h"

#include <fstream>
#include <boost/filesystem.hpp>
#include "json.hpp"

boost::filesystem::path Config::getConfigFilePath()
{
    TCHAR szPath[MAX_PATH];
    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
        return "";
    }
    boost::filesystem::path dir(szPath);
    dir = dir.parent_path().filename();
    boost::filesystem::path config("config.json");
    return dir / config;
}

Config::Config()
{
    defaultConfig = nlohmann::json();
    defaultConfig["width"] = Live2DWidth;
    defaultConfig["height"] = Live2DHeight;
}

BOOL Config::loadConfig()
{
    std::ifstream configFile(getConfigFilePath().c_str());
    if (configFile.good()) {
        nlohmann::json j;
        configFile >> j;
        try {
            Live2DWidth = j["width"];
            Live2DHeight = j["height"];
        } catch(std::exception ex) {
            OutputDebugString(L"Failed to load config, recreating a new config file...\n");
            return saveConfig();
        }
    }
    else {
        OutputDebugString(L"Config file is not existing, creating a new config file...\n");
        return saveConfig();
    }
    return TRUE;
}

BOOL Config::saveConfig()
{
    nlohmann::json j;
    j["width"] = Live2DWidth;
    j["height"] = Live2DHeight;
    std::ofstream configFile(getConfigFilePath().c_str());
    configFile << j.dump();
    return TRUE;
}

BOOL Config::resetConfig() {
    Live2DWidth = defaultConfig["width"];
    Live2DHeight = defaultConfig["height"];
    return saveConfig();
}
