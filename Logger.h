#pragma once
#include <filesystem>

namespace fs = std::filesystem;

enum class LogLevel {
	Silent,
	Log,
	Warn
};

enum class LogSeverity {
	Message,
	Warning,
	Error
};

class WhatWide {
	wchar_t* m_what = nullptr;
public:
	WhatWide(const std::exception& exc);
	const wchar_t* what() noexcept;
	~WhatWide();
};

namespace Logger {
	LogLevel getLogLevel();
	void setLogLevel(LogLevel);
	void setLogPath(fs::path path);
	const fs::path& getLogPath();
	void log(LogSeverity, const wchar_t* fmt, ...);
	void log_message(const wchar_t* fmt, ...);
	void log_warning(const wchar_t* fmt, ...);
	void log_error(const wchar_t* fmt, ...);
}