#pragma once
#include <Windows.h>

constexpr uintptr_t LM_RENDER_LEVEL_FUNCTION = 0x538876;
using renderLevelFunction = void(*)(DWORD a, DWORD b, DWORD c);

constexpr uintptr_t LM_MAP16_SAVE_FUNCTION = 0x440780;
using saveMap16Function = BOOL(*)();

constexpr uintptr_t LM_LEVEL_SAVE_FUNCTION = 0x46b5f0;
using saveLevelFunction = BOOL(*)(DWORD x);

constexpr uintptr_t LM_OW_SAVE_FUNCTION = 0x509ac0;
using saveOWFunction = BOOL(*)();

constexpr uintptr_t LM_NEW_ROM_FUNCTION = 0x467210;
using newRomFunction = BOOL(*)(DWORD a, DWORD b);

constexpr uintptr_t LM_TITLESCREEN_SAVE_FUNCTION = 0x4a3530;
using saveTitlescreenFunction = BOOL(*)();

constexpr uintptr_t LM_CREDITS_SAVE_FUNCTION = 0x4a3a20;
using saveCreditsFunction = BOOL(*)();

constexpr uintptr_t LM_SHARED_PALETTES_SAVE_FUNCTION = 0x44fd10;
using saveSharedPalettesFunction = BOOL(*)(BOOL x);

constexpr uintptr_t LM_EXPORT_ALL_MAP16_FUNCTION = 0x4CA8C0;
using export_all_map16_function = BOOL(*)(DWORD x, const char* full_output_path);

template <typename T>
constexpr T AddressToFnPtr(const uintptr_t address) {
	return reinterpret_cast<T>(address);
}
