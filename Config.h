#pragma once

#include <filesystem>
#include <string>
#include <array>
#include <tuple>
#include "Logger.h"

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
	const fs::path& getLogFilePath() const;
	LogLevel getLogLevel() const;
private:
	enum class Optional : bool {
		Yes = true,
		No = false
	};
	enum class Set : bool {
		Yes = true,
		No = false
	};
	using OptionTuple = std::tuple<const std::string_view, Optional, Set>;
	
	static inline std::array<OptionTuple, 9> configOptions{ {
		{"level_directory:"sv, Optional::No, Set::No},
		{"mwl_file_format:"sv, Optional::No, Set::No},
		{"flips_path:"sv, Optional::No, Set::No},
		{"map16_path:"sv, Optional::No, Set::No},
		{"clean_rom_path:"sv, Optional::No, Set::No},
		{"global_data_path:"sv, Optional::No, Set::No},
		{"shared_palettes_path:"sv, Optional::No, Set::No},
		{"log_path:"sv, Optional::Yes, Set::No},
		{"log_level:"sv, Optional::Yes, Set::No}
	}};

	static inline const std::string_view& levelDirectoryOption = std::get<const std::string_view>(configOptions[0]);
	static inline const std::string_view& mwlFormatOption = std::get<const std::string_view>(configOptions[1]);
	static inline const std::string_view& flipsPathOption = std::get<const std::string_view>(configOptions[2]);
	static inline const std::string_view& map16PathOption = std::get<const std::string_view>(configOptions[3]);
	static inline const std::string_view& cleanRomPathOption = std::get<const std::string_view>(configOptions[4]);
	static inline const std::string_view& globalDataPathOption = std::get<const std::string_view>(configOptions[5]);
	static inline const std::string_view& sharedPalettesPathOption = std::get<const std::string_view>(configOptions[6]);
	static inline const std::string_view& logFilePathOption = std::get<const std::string_view>(configOptions[7]);
	static inline const std::string_view& logLevelOption = std::get<const std::string_view>(configOptions[8]);

	fs::path levelDirectory;
	fs::path flipsPath;
	fs::path map16Path;
	fs::path cleanRomPath;
	fs::path sharedPalettesPath;
	fs::path globalDataPath;
	std::string mwlFileFormat;

	void setConfigVar(const std::string& varName, const std::string& varVal, const fs::path& basePath);
};
