#include "OnSharedPalettesSave.h"

void OnSharedPalettesSave::onSharedPalettesSave(bool succeeded, LM& lm, const std::optional<const Config>& config)
{
	if (succeeded && config.has_value())
	{
		onSuccessfulSharedPalettesSave(lm, config.value());
	}
	else
	{
		onFailedSharedPalettesSave(lm);
	}
}

void OnSharedPalettesSave::onSuccessfulSharedPalettesSave(LM& lm, const Config& config)
{
	try {
		fs::path romPath = lm.getPaths().getRomDir();
		romPath += lm.getPaths().getRomName();

		exportSharedPalettes(romPath, config.getSharedPalettesPath(), lm.getPaths().getLmExePath());
	}
	catch (const std::exception&)
	{

	}
}

void OnSharedPalettesSave::onFailedSharedPalettesSave(LM& lm)
{

}

void OnSharedPalettesSave::exportSharedPalettes(const fs::path& sourceRom, const fs::path& sharedPalettesPath, const fs::path& lmExePath)
{
	std::wstringstream ws;

	ws << '\"' << lmExePath.wstring() << "\" -ExportSharedPalette \"" << sourceRom.wstring() << "\" \"" << sharedPalettesPath.wstring() << "\"";

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
		throw std::runtime_error("Failed to export shared palettes");
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode;

	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (exitCode != 0)
	{
		throw std::runtime_error("Failed to export shared palettes");
	}
}
