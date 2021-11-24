#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

class Logger
{
	std::ofstream m_file;
	bool          m_bPrintToFile   = false;
	bool          m_bPrintToScreen = false;

public:
	Logger() noexcept = default;

	Logger(const std::string &sFilename, bool bPrintToFile = true, bool bPrintToScreen = true, bool bClearLog = false) noexcept
	{
		std::ios_base::openmode mode = bClearLog ? std::ios_base::out : std::ios_base::app;
		m_file = std::ofstream(sFilename, mode);

		m_bPrintToFile   = bPrintToFile;
		m_bPrintToScreen = bPrintToScreen;
	}

private:
	static std::stringstream getLocalTimeAsString() noexcept
	{
		const auto currentTime = std::chrono::system_clock::now();
		const auto asTimeType = std::chrono::system_clock::to_time_t(currentTime);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&asTimeType), "[%Y-%m-%d][%X] ");

		return ss;
	}

	std::string createString(const std::string &sText) const noexcept
	{
		std::stringstream ss = getLocalTimeAsString();
		ss << sText << '\n';

		return ss.str();
	}

public:
	void log(const std::string &sText) noexcept
	{
		std::string sFormatted = createString(sText);

		if (m_bPrintToScreen) std::cout << sFormatted;
		if (m_bPrintToFile) m_file << sFormatted;
	}

	void log(const std::string sBranch, const std::string &sText) noexcept
	{
		log('[' + sBranch + "] " + sText);
	}
};
