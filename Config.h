#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class Config
{
public:
	Config(const fs::path& configFilePath);

	const fs::path& getLevelDirectory() const;
	const fs::path& getFlipsPath() const;
	const fs::path& getMap16Path() const;
	const fs::path& getCleanRomPath() const;
	const fs::path& getSharedPalettesPath() const;
	const fs::path& getGlobalDataPath() const;
	const std::string& getMwlFileFormat() const;
private:
	fs::path levelDirectory;
	fs::path flipsPath;
	fs::path map16Path;
	fs::path cleanRomPath;
	fs::path sharedPalettesPath;
	fs::path globalDataPath;
	std::string mwlFileFormat;

	void setConfigVar(const std::string& varName, const std::string& varVal, const fs::path& basePath);
};
