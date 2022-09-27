#pragma once

#include <filesystem>
#include <cstdint>
#include <Windows.h>

constexpr uintptr_t LM_CURR_ROM_NAME = 0x5D0800;
constexpr uintptr_t LM_CURR_ROM_PATH = 0x7C7640;
constexpr uintptr_t LM_EXE_PATH = 0x5A1D38;
constexpr uintptr_t LM_TOOLBAR_HANDLE = 0xDD2464;
constexpr uintptr_t LM_MAIN_EDITOR_WINDOW_HANDLE = 0x8CD4F4;
constexpr uintptr_t LM_MAIN_STATUSBAR_HANDLE = 0xDD2458;

class Paths
{
public:
	static const char* getRomName();
	static const char* getRomDir();
	static const char* getLmExePath();
	static std::filesystem::path getRomPath();
	static HWND* getToolbarHandle();
	static HWND* getMainEditorWindowHandle();
	static HWND* getMainEditorStatusbarHandle();
};
