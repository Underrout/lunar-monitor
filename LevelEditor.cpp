#include "LevelEditor.h"
#include "LMFunctions.h"

#include <sstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

unsigned int LevelEditor::getCurrLevelNumber()
{
	return *reinterpret_cast<unsigned int*>(LM_CURR_LEVEL_NUMBER);
}

unsigned int LevelEditor::getLevelNumberBeingSaved()
{
	return *reinterpret_cast<unsigned int*>(LM_CURR_LEVEL_NUMBER_BEING_SAVED);
}

bool LevelEditor::exportMwl(
	const fs::path& lmExePath, const fs::path& romPath, 
	const fs::path& mwlFilePath, unsigned int levelNumber
)
{
	std::wstringstream ws;

	ws << '\"' << lmExePath.wstring() << "\" -ExportLevel \"" << romPath.wstring() <<
		"\" \"" << mwlFilePath.wstring() << "\" " << std::hex << levelNumber;

	std::wstring command = ws.str();
	std::vector<wchar_t> buf(command.begin(), command.end());
	buf.push_back(0);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL, buf.data(), NULL, NULL, false, 0, NULL, NULL, &si, &pi))
	{
		return false;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode;

	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return exitCode == 0;
}

bool LevelEditor::exportMap16(const fs::path& map16Path)
{
	return AddressToFnPtr<export_all_map16_function>(LM_EXPORT_ALL_MAP16_FUNCTION)(0, map16Path.string().c_str());
}
