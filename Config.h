#pragma once

#include <filesystem>
#include <string>
#include <array>
#include <tuple>

namespace fs = std::filesystem;

using namespace std::string_view_literals;

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
	static constexpr inline std::array<std::tuple<std::string_view, size_t>, 7> configOptions{ {
		{"level_directory:"sv, 0},
		{"mwl_file_format:"sv, 1},
		{"flips_path:"sv, 2},
		{"map16_path:"sv, 3},
		{"clean_rom_path:"sv, 4},
		{"global_data_path:"sv, 5},
		{"shared_palettes_path:"sv, 6}
	}};

	static constexpr const std::string_view& levelDirectoryOption = std::get<0>(configOptions[0]);
	static constexpr const std::string_view& mwlFormatOption = std::get<0>(configOptions[1]);
	static constexpr const std::string_view& flipsPathOption = std::get<0>(configOptions[2]);
	static constexpr const std::string_view& map16PathOption = std::get<0>(configOptions[3]);
	static constexpr const std::string_view& cleanRomPathOption = std::get<0>(configOptions[4]);
	static constexpr const std::string_view& globalDataPathOption = std::get<0>(configOptions[5]);
	static constexpr const std::string_view& sharedPalettesPathOption = std::get<0>(configOptions[6]);


	fs::path levelDirectory;
	fs::path flipsPath;
	fs::path map16Path;
	fs::path cleanRomPath;
	fs::path sharedPalettesPath;
	fs::path globalDataPath;
	std::string mwlFileFormat;

	void setConfigVar(const std::string& varName, const std::string& varVal, const fs::path& basePath);
};
