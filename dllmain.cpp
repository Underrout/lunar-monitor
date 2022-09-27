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

#include "BuildResultUpdater.h"

constexpr const char* FISH_REPLACEMENT = "   Mario says     TRANS RIGHTS  ";
constexpr const char* FISH = "I am Naaall, and I love fiiiish!";

constexpr const WORD IDM_EXPORT_ALL_BTN = 0x5BF9;

constexpr const size_t MAIN_EDITOR_STATUS_BAR_PARTS = 2;
constexpr const size_t SECOND_STATUSBAR_FIELD_WIDTH = 800;

constexpr const char* CONFIG_FILE_PATH = "lunar-monitor-config.txt";

std::optional<Config> config = std::nullopt;
LM lm{};

HMODULE g_hModule;

HWND gLmHandle;
DWORD verificationCode;

std::optional<std::string> lastRomBuildTime = std::nullopt;

HANDLE lunarHelperDirChangeWaiter;
HANDLE lunarHelperDirChange;

VOID InitFunction(DWORD a, DWORD b, DWORD c);

BOOL SaveLevelFunction(DWORD x, DWORD y);
BOOL SaveMap16Function();
BOOL SaveOWFunction();
BOOL NewRomFunction(DWORD a, DWORD b);
BOOL SaveCreditsFunction();
BOOL SaveTitlescreenFunction();
BOOL SaveSharedPalettesFunction(BOOL x);

void WriteCommentFieldFunction(uint32_t a, const char* comment, uint32_t b);

auto LMRenderLevelFunction = AddressToFnPtr<renderLevelFunction>(LM_RENDER_LEVEL_FUNCTION);

auto LMSaveLevelFunction = AddressToFnPtr<saveLevelFunction>(LM_LEVEL_SAVE_FUNCTION);
auto LMSaveMap16Function = AddressToFnPtr<saveMap16Function>(LM_MAP16_SAVE_FUNCTION);
auto LMSaveOWFunction = AddressToFnPtr<saveOWFunction>(LM_OW_SAVE_FUNCTION);
auto LMNewRomFunction = AddressToFnPtr<newRomFunction>(LM_NEW_ROM_FUNCTION);
auto LMSaveCreditsFunction = AddressToFnPtr<saveCreditsFunction>(LM_CREDITS_SAVE_FUNCTION);
auto LMSaveTitlescreenFunction = AddressToFnPtr<saveTitlescreenFunction>(LM_TITLESCREEN_SAVE_FUNCTION);
auto LMSaveSharedPalettesFunction = AddressToFnPtr<saveSharedPalettesFunction>(LM_SHARED_PALETTES_SAVE_FUNCTION);
auto LMWritecommentFunction = AddressToFnPtr<comment_field_write_function>(LM_COMMENT_FIELD_WRITE_FUNCTION);

HWND mainEditorProc;

void DllAttach(HMODULE hModule);
void DllDetach(HMODULE hModule);

void SetConfig(const fs::path& basePath);

void AddExportAllButton(HMODULE hModule);
void UpdateExportAllButton();
void AddStatusBarField();

void WatchLunarHelperDirectory();
void CALLBACK OnLunarHelperDirChange(_In_  PVOID unused, _In_  BOOLEAN TimerOrWaitFired);

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
    g_hModule = hModule;

    HANDLE pipe = CreateFile(
        L"\\\\.\\pipe\\lunar_monitor_pipe",
        GENERIC_READ, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (pipe != INVALID_HANDLE_VALUE)
    {
        ReadFile(
            pipe,
            &gLmHandle,
            sizeof(HWND),
            NULL,
            NULL
        );

        ReadFile(
            pipe,
            &verificationCode,
            sizeof(DWORD),
            NULL,
            NULL
        );
    }
    CloseHandle(pipe);

    DisableThreadLibraryCalls(hModule);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)LMRenderLevelFunction, InitFunction);
    DetourTransactionCommit();
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
    DetourDetach(&(PVOID&)LMWritecommentFunction, WriteCommentFieldFunction);
    DetourTransactionCommit();

    if (lunarHelperDirChangeWaiter != nullptr)
        UnregisterWait(lunarHelperDirChangeWaiter);
    if (lunarHelperDirChange != nullptr)
        FindCloseChangeNotification(lunarHelperDirChange);
}

VOID InitFunction(DWORD a, DWORD b, DWORD c)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)LMRenderLevelFunction, InitFunction);

    DetourAttach(&(PVOID&)LMSaveLevelFunction, SaveLevelFunction);
    DetourAttach(&(PVOID&)LMSaveMap16Function, SaveMap16Function);
    DetourAttach(&(PVOID&)LMSaveOWFunction, SaveOWFunction);
    DetourAttach(&(PVOID&)LMNewRomFunction, NewRomFunction);
    DetourAttach(&(PVOID&)LMSaveCreditsFunction, SaveCreditsFunction);
    DetourAttach(&(PVOID&)LMSaveTitlescreenFunction, SaveTitlescreenFunction);
    DetourAttach(&(PVOID&)LMSaveSharedPalettesFunction, SaveSharedPalettesFunction);
    DetourAttach(&(PVOID&)LMWritecommentFunction, WriteCommentFieldFunction);
    DetourTransactionCommit();

    AddStatusBarField();

    SetConfig(lm.getPaths().getRomDir());

    AddExportAllButton(g_hModule);

    if (config.has_value())
        WatchLunarHelperDirectory();

    LMRenderLevelFunction(a, b, c);
}

LRESULT CALLBACK MainEditorReplacementWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{
    if (uMsg == WM_COMMAND && wParam == IDM_EXPORT_ALL_BTN) {
        Logger::log_message(L"Export all button pressed, attempting to export all now");

        try {
            OnGlobalDataSave::exportBps(lm, config.value());
        }
        catch (const std::exception& exc)
        {
            WhatWide what{ exc };
            Logger::log_error(L"Full export failed: Global data export failed with exception: \"%s\"", what.what());

            return CallWindowProc((WNDPROC)mainEditorProc, *lm.getPaths().getMainEditorWindowHandle(), uMsg, wParam, lParam);
        }

        try {
            fs::path mwlPath = config.value().getLevelDirectory();
            const fs::path origPath = fs::path(mwlPath);
            mwlPath /= "level";
            fs::path romPath = lm.getPaths().getRomDir();
            romPath += lm.getPaths().getRomName();
            lm.getLevelEditor().exportAllMwls(lm.getPaths().getLmExePath(), romPath, mwlPath);

            Logger::log_message(L"Successfully exported all mwls to \"%s\"", mwlPath.c_str());
        }
        catch (const std::exception& exc)
        {
            WhatWide what{ exc };
            Logger::log_error(L"Full export failed: Export of all mwls failed with exception: \"%s\"", what.what());

            return CallWindowProc((WNDPROC)mainEditorProc, *lm.getPaths().getMainEditorWindowHandle(), uMsg, wParam, lParam);
        }

        if (!OnMap16Save::onSuccessfulMap16Save(lm, config.value()))
        {
            Logger::log_error(L"Full export failed: Map16 export failed, check log for details");
            return CallWindowProc((WNDPROC)mainEditorProc, *lm.getPaths().getMainEditorWindowHandle(), uMsg, wParam, lParam);
        }

        try {
            fs::path romPath = lm.getPaths().getRomDir();
            romPath += lm.getPaths().getRomName();

            OnSharedPalettesSave::exportSharedPalettes(romPath, config.value().getSharedPalettesPath(), lm.getPaths().getLmExePath());

            Logger::log_message(L"Successfully exported shared palettes to \"%s\"", config.value().getSharedPalettesPath().c_str());
        }
        catch (const std::runtime_error& err)
        {
            WhatWide what{ err };
            Logger::log_error(L"Full export failed: Shared palettes export failed with exception: \"%s\"", what.what());

            return CallWindowProc((WNDPROC)mainEditorProc, *lm.getPaths().getMainEditorWindowHandle(), uMsg, wParam, lParam);
        }

        Logger::log_message(L"Successfully exported all!");
    }

    return CallWindowProc((WNDPROC)mainEditorProc, *lm.getPaths().getMainEditorWindowHandle(), uMsg, wParam, lParam);
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

    TBBUTTON sep = { I_IMAGENONE, 0, 0, BTNS_SEP, { 0 }, 0, 0 };

    SendMessage(toolbarHandle, TB_SETMAXTEXTROWS, 0, 0);
    SendMessage(toolbarHandle, TB_INSERTBUTTON, 5, (LPARAM)&button);
    SendMessage(toolbarHandle, TB_INSERTBUTTON, 5, (LPARAM)&sep);
    SendMessage(toolbarHandle, TB_AUTOSIZE, 0, 0);

    mainEditorProc = (HWND)SetWindowLong(*(lm.getPaths().getMainEditorWindowHandle()), GWL_WNDPROC, (LONG)MainEditorReplacementWndProc);
}

void WriteCommentFieldFunction(uint32_t write_location, const char* comment, uint32_t comment_length)
{
    if (config.has_value() && strcmp(comment, FISH) == 0)
    {
        LMWritecommentFunction(write_location, FISH_REPLACEMENT, comment_length);
        return;
    }

    LMWritecommentFunction(write_location, comment, comment_length);
}

void UpdateExportAllButton()
{
    SendMessage(*(lm.getPaths().getToolbarHandle()), TB_INDETERMINATE, IDM_EXPORT_ALL_BTN, (LPARAM) MAKELONG(!config.has_value(), 0));
    SendMessage(*(lm.getPaths().getToolbarHandle()), TB_ENABLEBUTTON, IDM_EXPORT_ALL_BTN, (LPARAM)MAKELONG(config.has_value(), 0));
}

void WatchLunarHelperDirectory()
{
    if (lunarHelperDirChangeWaiter != nullptr)
    {
        UnregisterWait(lunarHelperDirChangeWaiter);
        lunarHelperDirChangeWaiter = nullptr;
    }
    if (lunarHelperDirChange != nullptr)
        FindCloseChangeNotification(lunarHelperDirChange);

    fs::path lunarHelperDir = lm.getPaths().getRomDir();
    lunarHelperDir += ".lunar_helper";

    if (!fs::exists(lunarHelperDir) || !fs::is_directory(lunarHelperDir))
    {
        fs::create_directory(lunarHelperDir);
        SetFileAttributes(lunarHelperDir.c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
    else
    {
        std::optional<json> buildReport = BuildResultUpdater::readInJson();

        if (buildReport.has_value())
        {
            try
            {
                lastRomBuildTime = buildReport.value()["build_time"].dump();
            }
            catch (const json::exception&)
            {
                lastRomBuildTime = std::nullopt;
            }
        }
        else
        {
            lastRomBuildTime = std::nullopt;
        }
    }

    lunarHelperDirChange = FindFirstChangeNotification(lunarHelperDir.c_str(), false, FILE_NOTIFY_CHANGE_LAST_WRITE);

    if (lunarHelperDirChange != INVALID_HANDLE_VALUE)
    {
        RegisterWaitForSingleObject(&lunarHelperDirChangeWaiter, lunarHelperDirChange, &OnLunarHelperDirChange, NULL, INFINITE, WT_EXECUTEONLYONCE);
    }
}

void CALLBACK OnLunarHelperDirChange(_In_  PVOID unused, _In_  BOOLEAN TimerOrWaitFired)
{
    std::optional<json> buildReport = BuildResultUpdater::readInJson();

    UnregisterWait(lunarHelperDirChangeWaiter);
    lunarHelperDirChangeWaiter = nullptr;

    if (buildReport.has_value())
    {
        try
        {
            std::string newHash = buildReport.value()["build_time"].dump();

            if (!lastRomBuildTime.has_value() || newHash != lastRomBuildTime.value())
            {
                FindCloseChangeNotification(lunarHelperDirChange);
                lunarHelperDirChange = nullptr;
                lastRomBuildTime = newHash;
                Logger::log_message(L"Change in Lunar Helper directory detected, reloading ROM...");
                lm.getLevelEditor().reloadROM(gLmHandle, verificationCode);
                return;
            }
        }
        catch (const json::exception&)
        {
            // pass
        }
    }

    RegisterWaitForSingleObject(&lunarHelperDirChangeWaiter, lunarHelperDirChange, &OnLunarHelperDirChange, NULL, INFINITE, WT_EXECUTEONLYONCE);

    FindNextChangeNotification(lunarHelperDirChange);
}

BOOL NewRomFunction(DWORD a, DWORD b)
{
    Logger::log_message(L"Attempting to switch to new ROM");

    BOOL result = LMNewRomFunction(a, b);

    if (result) 
    {
        fs::path romPath = lm.getPaths().getRomDir();
        romPath += lm.getPaths().getRomName();

        Logger::log_message(L"Successfully loaded ROM: \"%s\"", romPath.c_str());

        fs::current_path(lm.getPaths().getRomDir());
        SetConfig(lm.getPaths().getRomDir());

        UpdateExportAllButton();

        if (config.has_value())
            WatchLunarHelperDirectory();
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

BOOL SaveLevelFunction(DWORD x, DWORD y)
{
    BOOL succeeded = LMSaveLevelFunction(x, y);

    OnLevelSave::onLevelSave(succeeded, lm.getLevelEditor().getLevelNumberBeingSaved(), lm, config);

    return succeeded;
}

BOOL SaveMap16Function()
{
    __asm {
        mov eax,ebx
    }

    BOOL succeeded = LMSaveMap16Function();

    OnMap16Save::onMap16Save(succeeded, lm, config);

    return succeeded;
}

BOOL SaveOWFunction()
{
    __asm {
        mov eax,[ebp]
    }
    BOOL succeeded = LMSaveOWFunction();

    OnGlobalDataSave::onGlobalDataSave(succeeded, lm, config);

    return succeeded;
}

BOOL SaveTitlescreenFunction()
{
    __asm {
        mov eax,edi
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
        mov eax,esi
        push x
        call LMSaveSharedPalettesFunction
        mov succeeded,eax
    }

    OnSharedPalettesSave::onSharedPalettesSave(succeeded, lm, config);

    return succeeded;
}
