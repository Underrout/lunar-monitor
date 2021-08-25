#include "Logger.h"
#include <cstdarg>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <memory>

namespace Logger {

	class TheLogger {
	private:
		mutable bool m_has_logged_once = false;
		LogLevel m_level;
		mutable std::ofstream m_file;
		fs::path m_filepath;
		mutable std::vector<std::string> m_logs;
		static constexpr size_t s_threshold = 10;

		void open_log_file() const {
			m_file.open(m_filepath, std::ofstream::out | std::ofstream::app);
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
			std::stringstream stream{};
			auto t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			stream << std::put_time(std::localtime(&t_c), "Start of log: [ %F %T ]\n");
			m_logs.push_back(stream.str());
		}
		void log(const char* fmt, va_list argptr) const {
			m_has_logged_once = true;
			switch (m_level) {
			case LogLevel::Warn:
				// TODO: print on screen here
			case LogLevel::Log:
			{
				auto ret = _vscprintf_l(fmt, nullptr, argptr);
				char* buf = new char[ret + 1];
				vsprintf(buf, fmt, argptr);
				auto t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::stringstream stream{};
				stream << std::put_time(std::localtime(&t_c), "[ %F %T ] - ") << buf << '\n';
				m_logs.push_back(stream.str());
				if (m_logs.size() == s_threshold)
					flush_logs_to_file();
			}
			break;
			default:
				break;
			}
		}
		void setLogLevel(LogLevel level) {
			level = m_level;
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

	void log(const char* fmt, ...) {
		va_list lst{};
		va_start(lst, fmt);
		getLoggerInstance()->log(fmt, lst);
		va_end(lst);
	}

	// TODO: this api doesn't really make sense
	void setLogPath(fs::path path) {
		getLoggerInstance()->setLogPath(std::forward<fs::path>(path));
	}

	const fs::path& getLogPath() {
		return getLoggerInstance()->getLogPath();
	}
}
