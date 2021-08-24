#pragma once

#include "LM.h"
#include "Config.h"

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class OnMap16Save
{
public:
	static void onMap16Save(bool succeeded, LM& lm, const std::optional<const Config>& config);
private:
	static void onSuccessfulMap16Save(LM& lm, const Config& config);
	static void onFailedMap16Save(LM& lm);
};
