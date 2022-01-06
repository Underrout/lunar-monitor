#include <Windows.h>
#include <detours.h>

#include <iostream>
#include "OnLevelSave.h"
#include "OnMap16Save.h"
#include "OnGlobalDataSave.h"
#include "OnSharedPalettesSave.h"

#include "LMFunctions.h"
#include "LM.h"
#include "Config.h"

constexpr const char* CONFIG_FILE_PATH = "lunar-monitor-config.txt";

std::optional<Config> config = std::nullopt;
LM lm{};

BOOL SaveLevelFunction(DWORD x, DWORD y);
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

void DllAttach(HMODULE hModule);
void DllDetach(HMODULE hModule);

void SetConfig(const fs::path& basePath);

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

    SetConfig(lm.getPaths().getRomDir());
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

BOOL NewRomFunction(DWORD a, DWORD b)
{
    Logger::log_message(L"Attempting to switch to new ROM");

    BOOL result = LMNewRomFunction(a, b);

    if (result) 
    {
        fs::path romPath = lm.getPaths().getRomDir();
        romPath += lm.getPaths().getRomName();

        Logger::log_message(L"Successfully switched to other ROM: \"%s\", goodbye", romPath.c_str());

        fs::current_path(lm.getPaths().getRomDir());
        SetConfig(lm.getPaths().getRomDir());

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

BOOL SaveLevelFunction(DWORD x, DWORD y)
{
    BOOL succeeded = LMSaveLevelFunction(x, y);

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
