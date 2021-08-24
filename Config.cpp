#include "Config.h"

Config::Config(const fs::path& configFilePath)
{
	const fs::path basePath = configFilePath.parent_path();

	std::regex configValDefinition("(level_directory|mwl_file_format|flips_path|map16_path|clean_rom_path|global_data_path|shared_palettes_path):\\s+\"(.+)\"");

	std::ifstream file(configFilePath);
	std::string currLine;

	std::unordered_set<std::string> seenVarNames{};

	while (std::getline(file, currLine))
	{
		if (currLine.substr(0, 2) == "--")
			continue;

		std::smatch matchResults;
		if (std::regex_match(currLine, matchResults, configValDefinition))
		{
			std::string varName = matchResults[1];
			std::string varVal = matchResults[2];

			if (seenVarNames.count(varName) == 0)
			{
				setConfigVar(varName, varVal, basePath);
				seenVarNames.insert(varName);
			}
			else
			{
				throw std::runtime_error("Config var redefined");
			}
		}
	}

	if (seenVarNames.size() != 7)
	{
		throw std::runtime_error("Not all config vars specified");
	}
}

void Config::setConfigVar(const std::string& varName, const std::string& varVal, const fs::path& basePath)
{
	if (varName == "level_directory")
	{
		levelDirectory = basePath;
		levelDirectory /= varVal;
	}
	else if (varName == "mwl_file_format")
	{
		mwlFileFormat = varVal;
	}
	else if (varName == "flips_path")
	{
		flipsPath = basePath;
		flipsPath /= varVal;
	}
	else if (varName == "map16_path")
	{
		map16Path = basePath;
		map16Path /= varVal;
	}
	else if (varName == "clean_rom_path")
	{
		cleanRomPath = basePath;
		cleanRomPath /= varVal;
	}
	else if (varName == "shared_palettes_path")
	{
		sharedPalettesPath = basePath;
		sharedPalettesPath /= varVal;
	}
	else if (varName == "global_data_path")
	{
		globalDataPath = basePath;
		globalDataPath /= varVal;
	}
	else
	{
		throw std::runtime_error("Invalid config var detected");
	}
}

const fs::path& Config::getLevelDirectory() const
{
	return levelDirectory;
}

const std::string& Config::getMwlFileFormat() const
{
	return mwlFileFormat;
}

const fs::path& Config::getMap16Path() const
{
	return map16Path;
}

const fs::path& Config::getFlipsPath() const
{
	return flipsPath;
}

const fs::path& Config::getGlobalDataPath() const
{
	return globalDataPath;
}

const fs::path& Config::getCleanRomPath() const
{
	return cleanRomPath;
}

const fs::path& Config::getSharedPalettesPath() const
{
	return sharedPalettesPath;
}
