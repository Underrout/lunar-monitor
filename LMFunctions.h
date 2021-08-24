#pragma once

#include <Windows.h>
#include <string>

#define LM_MAP16_SAVE_FUNCTION 0x440780
typedef BOOL(*saveMap16Function)();

#define LM_LEVEL_SAVE_FUNCTION 0x46b5f0
typedef BOOL(*saveLevelFunction)(DWORD x);

#define LM_OW_SAVE_FUNCTION 0x509ac0
typedef BOOL(*saveOWFunction)();

#define LM_NEW_ROM_FUNCTION 0x467210
typedef BOOL(*newRomFunction)(DWORD a, DWORD b);

#define LM_TITLESCREEN_SAVE_FUNCTION 0x4a3530
typedef BOOL(*saveTitlescreenFunction)();

#define LM_CREDITS_SAVE_FUNCTION 0x4a3a20
typedef BOOL(*saveCreditsFunction)();

#define LM_SHARED_PALETTES_SAVE_FUNCTION 0x44fd10
typedef BOOL(*saveSharedPalettesFunction)(BOOL x);
