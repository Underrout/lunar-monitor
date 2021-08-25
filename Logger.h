#pragma once
#include <filesystem>

namespace fs = std::filesystem;

enum class LogLevel {
	Silent,
	Log,
	Warn
};

namespace Logger {
	LogLevel getLogLevel();
	void setLogLevel(LogLevel);
	void setLogPath(fs::path path);
	const fs::path& getLogPath();
	void log(const char* fmt, ...);
}