#include "Logger.h"
#include <cstdarg>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <memory>
#include <Windows.h>

constexpr const wchar_t* MODULE_NAME = L"lunar-monitor.dll";

// just a quick class to prevent from having to think about allocation and deallocation of things manually
class FormatTempBuffer {
	wchar_t* m_buffer = nullptr;
public:
	FormatTempBuffer(const wchar_t* fmt, va_list argptr) {
		auto ret = _vscwprintf_l(fmt, nullptr, argptr);
		m_buffer = new wchar_t[ret + 1];
		vswprintf(m_buffer, ret + 1, fmt, argptr);
	}
	wchar_t* buffer() {
		return m_buffer;
	}

	~FormatTempBuffer() {
		delete[] m_buffer;
	}
};

WhatWide::WhatWide(const std::exception& exc) {
	const char* what = exc.what();
	size_t len = strlen(what);
	m_what = new wchar_t[len + 1];
	mbstowcs_s(nullptr, m_what, len + 1, what, len);
}

const wchar_t* WhatWide::what() noexcept
{
	return m_what;
}

WhatWide::~WhatWide()
{
	delete[] m_what;
}


// to be used only in dire situations.
#ifdef _DEBUG
void EmergencyLogToFile(const wchar_t* fmt, ...) {
	va_list lst{};
	va_start(lst, fmt);
	std::wofstream emergency{};
	emergency.open("emergency_log.txt", std::wofstream::out | std::wofstream::trunc);
	FormatTempBuffer buf{ fmt, lst };
	emergency << buf.buffer() << L'\n';
	emergency.close();
	va_end(lst);
}
#endif

namespace Logger {
#ifdef _DEBUG
	class TheLoggerConsole {
		constexpr static auto inline C_RED = FOREGROUND_RED | FOREGROUND_INTENSITY;
		constexpr static auto inline C_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
		constexpr static auto inline C_GREEN = FOREGROUND_GREEN;
		constexpr static auto inline C_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		HANDLE m_output;
	public:

		TheLoggerConsole() {
			if (!AllocConsole()) {
				// if we can't allocate the console we can't use the log file because this is called in its constructor
				// therefore the file handle is not guaranteed to exist and we'd invoke infinite recursion
				// use the emergency file
				auto lastError = GetLastError();
				auto hresult = HRESULT_FROM_WIN32(lastError);
				EmergencyLogToFile(L"Impossible to create console, error %X, HRESULT: %X", lastError, hresult);
			}
			m_output = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		HANDLE getConsoleHandle() {
			return m_output;
		}
		void WriteToConsole(LogSeverity severity, const wchar_t* msg, size_t length) const {
			switch (severity) {
			case LogSeverity::Message:
				SetConsoleTextAttribute(m_output, C_GREEN);
				break;
			case LogSeverity::Warning:
				SetConsoleTextAttribute(m_output, C_YELLOW);
				break;
			case LogSeverity::Error:
				SetConsoleTextAttribute(m_output, C_RED);
				break;
			default:
				break;
			}
			BOOL res = WriteConsole(m_output, msg, length + 1, nullptr, nullptr);
			if (!res) {
				log_error(L"Impossible to write to console, error %X", GetLastError());
			}
			SetConsoleTextAttribute(m_output, C_WHITE);
		}

		~TheLoggerConsole() {
			if (!FreeConsole()) {
				// if we can't free the console we can't use the log file because this is called in its destructor
				// therefore the file handle is not guaranteed to exist and we'd invoke undefined behavior and/or crashes
				// use the emergency file
				auto lastError = GetLastError();
				auto hresult = HRESULT_FROM_WIN32(lastError);
				EmergencyLogToFile(L"Impossible to free console, error %X, HRESULT: %X", lastError, hresult);
			}
		}
	};
#endif
	class TheLogger {
	private:
#ifdef _DEBUG
		TheLoggerConsole m_debug_console{};
#endif
		mutable bool m_has_logged_once = false;
		LogLevel m_level;
		mutable std::wofstream m_file;
		fs::path m_filepath;
		mutable std::vector<std::wstring> m_logs;
		static constexpr size_t s_threshold = 10;

		void open_log_file() const {
			m_file.open(m_filepath, std::wofstream::out | std::wofstream::app);
		}

		void close_log_file() const {
			m_file.close();
		}

		void flush_logs_to_file() const {
			open_log_file();
			for (auto& log : m_logs)
				m_file << log;
			close_log_file();
			m_logs.clear();
		}

	public:
		TheLogger() : m_level(LogLevel::Log), m_filepath(L"lunar_monitor_log.txt"), m_file() {
			std::wstringstream stream{};
			auto t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			struct tm time_now {};
			auto err = localtime_s(&time_now, &t_c);
			if (err == 0)
				stream << std::put_time(&time_now, L"Start of log: [ %F %T ]\n");
			m_logs.push_back(stream.str());
		}
		void log(LogSeverity severity, const wchar_t* fmt, va_list argptr) const {
			m_has_logged_once = true;
			switch (m_level) {
			case LogLevel::Warn:
				if (severity != LogSeverity::Message) {
					// TODO: find a way to make LM pop up a window with the message here
					// and continue as it was LogLevel::Log
				}
				[[fallthrough]];
			case LogLevel::Log:
			{
				FormatTempBuffer buf{ fmt, argptr };
				auto t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::wstringstream stream{};
				switch (severity) {
				case LogSeverity::Message:
					stream << L"[Message] - ";
					break;
				case LogSeverity::Warning:
					stream << L"[Warning] - ";
					break;
				case LogSeverity::Error:
					stream << L"[Error] - ";
					break;
				default:
					stream << L"[Unspecified] - ";
					break;
				}
				struct tm time_now {};
				auto err = localtime_s(&time_now, &t_c);
				if (err == 0)
					stream << std::put_time(&time_now, L"[ %F %T ] - ") << buf.buffer() << L'\n';
				m_logs.push_back(stream.str());
#ifdef _DEBUG
				m_debug_console.WriteToConsole(severity, m_logs.back().c_str(), m_logs.back().size());
#endif
				if (m_logs.size() == s_threshold)
					flush_logs_to_file();
			}
			break;
			default:
				break;
			}
		}
		void setLogLevel(LogLevel level) {
			m_level = level;
		}

		void setLogPath(fs::path&& path) {
			flush_logs_to_file();
			if (!m_has_logged_once)
				fs::remove(m_filepath);
			m_filepath = std::move(path);
		}

		LogLevel getLogLevel() const {
			return m_level;
		}

		const fs::path& getLogPath() const {
			return m_filepath;
		}

		~TheLogger() {
			if (!m_file)
				return;
			if (m_level != LogLevel::Silent)
				flush_logs_to_file();
		}
	};

	static const std::unique_ptr<TheLogger>& getLoggerInstance()
	{
		static auto sLoggerInstance = std::make_unique<TheLogger>();
		return sLoggerInstance;
	}

	LogLevel getLogLevel()
	{
		return getLoggerInstance()->getLogLevel();
	}

	void setLogLevel(LogLevel level) {
		getLoggerInstance()->setLogLevel(level);
	}

	void log(LogSeverity severity, const wchar_t* fmt, ...) {
		va_list lst{};
		va_start(lst, fmt);
		getLoggerInstance()->log(severity, fmt, lst);
		va_end(lst);
	}

	void log_message(const wchar_t* fmt, ...) {
		va_list lst{};
		va_start(lst, fmt);
		getLoggerInstance()->log(LogSeverity::Message, fmt, lst);
		va_end(lst);
	}
	void log_warning(const wchar_t* fmt, ...) {
		va_list lst{};
		va_start(lst, fmt);
		getLoggerInstance()->log(LogSeverity::Warning, fmt, lst);
		va_end(lst);
	}
	void log_error(const wchar_t* fmt, ...) {
		va_list lst{};
		va_start(lst, fmt);
		getLoggerInstance()->log(LogSeverity::Error, fmt, lst);
		va_end(lst);
	}

	void setLogPath(fs::path path) {
		getLoggerInstance()->setLogPath(std::forward<fs::path>(path));
	}

	const fs::path& getLogPath() {
		return getLoggerInstance()->getLogPath();
	}
}
