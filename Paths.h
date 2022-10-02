#pragma once

#include <filesystem>
#include <cstdint>
#include <Windows.h>

constexpr uintptr_t LM_CURR_ROM_NAME = 0x5D1800;
constexpr uintptr_t LM_CURR_ROM_PATH = 0x7C8740;
constexpr uintptr_t LM_EXE_PATH = 0x5A2D38;
constexpr uintptr_t LM_TOOLBAR_HANDLE = 0xDD4374;
constexpr uintptr_t LM_MAIN_EDITOR_WINDOW_HANDLE = 0x8CE5F4;
constexpr uintptr_t LM_MAIN_STATUSBAR_HANDLE = 0xDD4368;

constexpr size_t COMMENT_FIELD_SFC_ROM_OFFSET = 0x7F120;
constexpr size_t COMMENT_FIELD_SMC_ROM_OFFSET = 0x7F320;
constexpr const char* FISH_REPLACEMENT = "   Mario says     TRANS RIGHTS  ";
constexpr const char* FISH = "I am Naaall, and I love fiiiish!";

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
