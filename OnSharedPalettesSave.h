#pragma once
#include "LM.h"
#include "Config.h"

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class OnSharedPalettesSave
{
public:
	static void onSharedPalettesSave(bool succeeded, LM& lm, const std::optional<const Config>& config);
private:
	static void onSuccessfulSharedPalettesSave(LM& lm, const Config& config);
	static void onFailedSharedPalettesSave(LM& lm);
	static void exportSharedPalettes(const fs::path& sourceRom, const fs::path& sharedPalettesPath, const fs::path& lmExePath);
};
