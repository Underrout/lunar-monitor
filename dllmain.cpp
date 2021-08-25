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

void DllAttach(HMODULE hModule);
void DllDetach(HMODULE hModule);

void SetConfig(const fs::path& basePath);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            Logger::log_message(L"Attaching to process...");
            DllAttach(hModule);
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            Logger::log_message(L"Detaching from process...");
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
    BOOL result = LMNewRomFunction(a, b);

    if (result) 
    {
        fs::current_path(lm.getPaths().getRomDir());
        SetConfig(lm.getPaths().getRomDir());
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
    }
    catch (const std::runtime_error& err)
    {
        WhatWide what{ err };
        Logger::log_error(L"Failed to setup configuration file, error was \"%s\"", what.what());
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
    BOOL succeeded = LMSaveSharedPalettesFunction(x);

    OnSharedPalettesSave::onSharedPalettesSave(succeeded, lm, config);

    return succeeded;
}
