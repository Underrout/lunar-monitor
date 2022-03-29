#include <Windows.h>
#include <detours.h>
#include <CommCtrl.h>
#pragma comment (lib, "comctl32")

#include <iostream>
#include "OnLevelSave.h"
#include "OnMap16Save.h"
#include "OnGlobalDataSave.h"
#include "OnSharedPalettesSave.h"

#include "LMFunctions.h"
#include "LM.h"
#include "resource.h"
#include "Config.h"

constexpr const WORD IDM_EXPORT_ALL_BTN = 0x5BF9;

constexpr const size_t MAIN_EDITOR_STATUS_BAR_PARTS = 2;
constexpr const size_t SECOND_STATUSBAR_FIELD_WIDTH = 800;

constexpr const char* CONFIG_FILE_PATH = "lunar-monitor-config.txt";

std::optional<Config> config = std::nullopt;
LM lm{};

BOOL SaveLevelFunction(DWORD x);
BOOL SaveMap16Function();
BOOL SaveOWFunction();
BOOL NewRomFunction(DWORD a, DWORD b);
BOOL SaveCreditsFunction();
BOOL SaveTitlescreenFunction();
BOOL SaveSharedPalettesFunction(BOOL x);

auto LMSaveLevelFunction = AddressToFnPtr<saveLevelFunction>(LM_LEVEL_SAVE_FUNCTION);
auto LMSaveMap16Function = AddressToFnPtr<saveMap16Function>(LM_MAP16_SAVE_FUNCTION);
auto LMSaveOWFunction = AddressToFnPtr<saveOWFunction>(LM_OW_SAVE_FUNCTION);
auto LMNewRomFunction = AddressToFnPtr<newRomFunction>(LM_NEW_ROM_FUNCTION);
auto LMSaveCreditsFunction = AddressToFnPtr<saveCreditsFunction>(LM_CREDITS_SAVE_FUNCTION);
auto LMSaveTitlescreenFunction = AddressToFnPtr<saveTitlescreenFunction>(LM_TITLESCREEN_SAVE_FUNCTION);
auto LMSaveSharedPalettesFunction = AddressToFnPtr<saveSharedPalettesFunction>(LM_SHARED_PALETTES_SAVE_FUNCTION);

HWND mainEditorProc;

void DllAttach(HMODULE hModule);
void DllDetach(HMODULE hModule);

void SetConfig(const fs::path& basePath);

void AddExportAllButton(HMODULE hModule);
void UpdateExportAllButton();
void AddStatusBarField();

LRESULT CALLBACK MainEditorReplacementWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam);    // second message parameter;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DllAttach(hModule);
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            DllDetach(hModule);
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

void DllAttach(HMODULE hModule)
{
    DisableThreadLibraryCalls(hModule);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)LMSaveLevelFunction, SaveLevelFunction);
    DetourAttach(&(PVOID&)LMSaveMap16Function, SaveMap16Function);
    DetourAttach(&(PVOID&)LMSaveOWFunction, SaveOWFunction);
    DetourAttach(&(PVOID&)LMNewRomFunction, NewRomFunction);
    DetourAttach(&(PVOID&)LMSaveCreditsFunction, SaveCreditsFunction);
    DetourAttach(&(PVOID&)LMSaveTitlescreenFunction, SaveTitlescreenFunction);
    DetourAttach(&(PVOID&)LMSaveSharedPalettesFunction, SaveSharedPalettesFunction);
    DetourTransactionCommit();

    AddStatusBarField();

    SetConfig(lm.getPaths().getRomDir());

    AddExportAllButton(hModule);
}

void DllDetach(HMODULE hModule)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)LMSaveLevelFunction, SaveLevelFunction);
    DetourDetach(&(PVOID&)LMSaveMap16Function, SaveMap16Function);
    DetourDetach(&(PVOID&)LMSaveOWFunction, SaveOWFunction);
    DetourDetach(&(PVOID&)LMNewRomFunction, NewRomFunction);
    DetourDetach(&(PVOID&)LMSaveCreditsFunction, SaveCreditsFunction);
    DetourDetach(&(PVOID&)LMSaveTitlescreenFunction, SaveTitlescreenFunction);
    DetourDetach(&(PVOID&)LMSaveSharedPalettesFunction, SaveSharedPalettesFunction);
    DetourTransactionCommit();
}

LRESULT CALLBACK MainEditorReplacementWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{
    if (uMsg == WM_COMMAND && wParam == IDM_EXPORT_ALL_BTN) {
        Logger::log_message(L"Export all button pressed, attempting to export all now");

        OnGlobalDataSave::onGlobalDataSave(true, lm, config);

        try {
            fs::path mwlPath = config.value().getLevelDirectory();
            const fs::path origPath = fs::path(mwlPath);
            mwlPath /= "level";
            fs::path romPath = lm.getPaths().getRomDir();
            romPath += lm.getPaths().getRomName();
            lm.getLevelEditor().exportAllMwls(lm.getPaths().getLmExePath(), romPath, mwlPath);

            Logger::log_message(L"Successfully exported all levels to \"%s\"", mwlPath.c_str());
        }
        catch (const std::exception& exc)
        {
            WhatWide what{ exc };
            Logger::log_error(L"Export of all mwls failed with exception: \"%s\"", what.what());
        }

        OnMap16Save::onMap16Save(true, lm, config);
        OnSharedPalettesSave::onSharedPalettesSave(true, lm, config);

        Logger::log_message(L"Successfully exported all!");
        return 0;
    }

    return CallWindowProc((WNDPROC)mainEditorProc, *(lm.getPaths().getMainEditorWindowHandle()), uMsg, wParam, lParam);
}

void AddStatusBarField()
{
    int parts[MAIN_EDITOR_STATUS_BAR_PARTS + 1];
    SendMessage(*lm.getPaths().getMainEditorStatusbarHandle(), SB_GETPARTS, MAIN_EDITOR_STATUS_BAR_PARTS, (LPARAM)&parts);

    parts[MAIN_EDITOR_STATUS_BAR_PARTS] = -1;
    parts[MAIN_EDITOR_STATUS_BAR_PARTS - 1] = SECOND_STATUSBAR_FIELD_WIDTH;

    SendMessage(*lm.getPaths().getMainEditorStatusbarHandle(), SB_SETPARTS, MAIN_EDITOR_STATUS_BAR_PARTS + 1, (LPARAM)&parts);
}

void AddExportAllButton(HMODULE hModule)
{
    HWND toolbarHandle = *(lm.getPaths().getToolbarHandle());

    DWORD backgroundColor = GetSysColor(COLOR_BTNFACE);
    COLORMAP colorMap;
    colorMap.from = RGB(0xFF, 0xFF, 0xFF);
    colorMap.to = backgroundColor;
    HBITMAP hbm = CreateMappedBitmap(hModule, IDB_BITMAP1, 0, &colorMap, 1);
    TBADDBITMAP tb;
    tb.hInst = NULL;
    tb.nID = (UINT_PTR)hbm;

    int index = SendMessage(toolbarHandle, TB_ADDBITMAP, 0, (LPARAM)&tb);

    const DWORD buttonStyles = TBSTYLE_AUTOSIZE;

    TBBUTTON button = { index, IDM_EXPORT_ALL_BTN, (BYTE)(config.has_value() ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE), 
        buttonStyles, { 0 }, 0, (INT_PTR)L"Export map16, all levels, global data and shared palettes for Lunar Helper"};

    SendMessage(toolbarHandle, TB_SETMAXTEXTROWS, 0, 0);
    SendMessage(toolbarHandle, TB_INSERTBUTTON, 5, (LPARAM)&button);
    SendMessage(toolbarHandle, TB_AUTOSIZE, 0, 0);

    lm.getPaths().getMainEditorWindowHandle();

    mainEditorProc = (HWND)SetWindowLong(*(lm.getPaths().getMainEditorWindowHandle()), GWL_WNDPROC, (LONG)MainEditorReplacementWndProc);
}

void UpdateExportAllButton()
{
    SendMessage(*(lm.getPaths().getToolbarHandle()), TB_INDETERMINATE, IDM_EXPORT_ALL_BTN, (LPARAM) MAKELONG(!config.has_value(), 0));
    SendMessage(*(lm.getPaths().getToolbarHandle()), TB_ENABLEBUTTON, IDM_EXPORT_ALL_BTN, (LPARAM)MAKELONG(config.has_value(), 0));
}

BOOL NewRomFunction(DWORD a, DWORD b)
{
    Logger::log_message(L"Attempting to switch to new ROM");

    BOOL result = LMNewRomFunction(a, b);

    if (result) 
    {
        fs::path romPath = lm.getPaths().getRomDir();
        romPath += lm.getPaths().getRomName();

        Logger::log_message(L"Successfully switched to other ROM: \"%s\"", romPath.c_str());

        fs::current_path(lm.getPaths().getRomDir());
        SetConfig(lm.getPaths().getRomDir());

        UpdateExportAllButton();

        Logger::log_message(L"Successfully loaded ROM: \"%s\"", romPath.c_str());
    }
    else
    {
        Logger::log_message(L"Failed to switch to new ROM");
    }

    return result;
}

void SetConfig(const fs::path& basePath) 
{
    fs::path configPath = basePath;
    configPath += CONFIG_FILE_PATH;

    try
    {
        config = Config(configPath);

        Logger::log_message(L"------- START OF LOG -------");
        Logger::log_message(L"Successfully loaded config file from \"%s\"", configPath.wstring().c_str());
    }
    catch (const std::runtime_error& err)
    {
        if (!fs::exists(configPath))
        {
            Logger::setLogLevel(LogLevel::Silent);
        }
        else
        {
            Logger::setDefaultLogLevel();
        }

        Logger::setDefaultLogPath(basePath);

        WhatWide what{ err };
        Logger::log_message(L"------- START OF LOG -------");
        Logger::log_error(L"Failed to setup configuration file, error was \"%s\"", what.what());
        config = std::nullopt;
    }
    catch (const std::exception& exc) 
    {
        if (!fs::exists(configPath))
        {
            Logger::setLogLevel(LogLevel::Silent);
        }
        else
        {
            Logger::setDefaultLogLevel();
        }

        Logger::setDefaultLogPath(basePath);

        WhatWide what{ exc };
        Logger::log_message(L"------- START OF LOG -------");
        Logger::log_error(L"Uncaught exception while reading config file, error was \"%s\"", what.what());
        config = std::nullopt;
    }

}

BOOL SaveLevelFunction(DWORD x)
{
    BOOL succeeded = LMSaveLevelFunction(x);

    OnLevelSave::onLevelSave(succeeded, lm.getLevelEditor().getLevelNumberBeingSaved(), lm, config);

    return succeeded;
}

BOOL SaveMap16Function()
{
    __asm {
        push ebp
        mov ebp,edi
        mov eax,edi
    }

    BOOL succeeded = LMSaveMap16Function();

    __asm {
        pop ebp
    }

    OnMap16Save::onMap16Save(succeeded, lm, config);

    return succeeded;
}

BOOL SaveOWFunction()
{
    BOOL succeeded = LMSaveOWFunction();

    OnGlobalDataSave::onGlobalDataSave(succeeded, lm, config);

    return succeeded;
}

BOOL SaveTitlescreenFunction()
{
    __asm {
        mov eax,[ebp]
    }
    BOOL succeeded = LMSaveTitlescreenFunction();

    OnGlobalDataSave::onGlobalDataSave(succeeded, lm, config);

    return succeeded;
}

BOOL SaveCreditsFunction()
{
    BOOL succeeded = LMSaveCreditsFunction();

    OnGlobalDataSave::onGlobalDataSave(succeeded, lm, config);

    return succeeded;
}

BOOL SaveSharedPalettesFunction(BOOL x)
{
    BOOL succeeded;
    __asm {
        push x
        mov eax,esi
        CALL LMSaveSharedPalettesFunction
        mov succeeded,eax
    }

    OnSharedPalettesSave::onSharedPalettesSave(succeeded, lm, config);

    return succeeded;
}
