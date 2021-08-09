// lunar-monitor-prototype-window.cpp : Defines the entry point for the application.
//

#define DEBUG

#include "framework.h"

#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <cstdio>

#ifndef WINVER                // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501        // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                  
#define _WIN32_WINNT 0x0501    // Change this to the appropriate value to target other versions of Windows.
#endif                       

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers

#define MAX_LOADSTRING 100
#define SZ_TITLE "MinWinApp"
#define SZ_WND_CLASS L"MINWINAPP"


// Global Variables:
HINSTANCE g_hInst;                // current instance
HWND g_hWnd;
HWND g_hwndNextViewer;

unsigned int lvlNum{ 0x105 };

LPCWSTR szWndClass = SZ_WND_CLASS;

#ifdef DEBUG
bool RedirectConsoleIO();
bool CreateNewConsole(int16_t minLength);
void AdjustConsoleBuffer(int16_t minLength);
#endif

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void handlePotentialROMChange();

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef DEBUG
    CreateNewConsole(500);
#endif

    MSG msg;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWndClass;
    wcex.hIconSm = NULL;
    RegisterClassEx(&wcex);

    g_hInst = hInstance; // Store instance handle in our global variable
    g_hWnd = CreateWindowExW(0, szWndClass, szWndClass, 0, 0, 0, 0, 0, 0, 0, hInstance, 0);

    if (!g_hWnd)
    {
        return FALSE;
    }

    ShowWindow(g_hWnd, SW_HIDE);

    HANDLE romChangeHandle = FindFirstChangeNotification(
        L".", false, FILE_NOTIFY_CHANGE_LAST_WRITE
    );

    while (true)
    {
        DWORD result = WaitForSingleObject(romChangeHandle, 50);

        if (result == WAIT_OBJECT_0) 
        {
#ifdef DEBUG
            std::cout << "Change in ROM dir detected" << std::endl;
#endif
            handlePotentialROMChange();
        }

        FindNextChangeNotification(romChangeHandle);

        BOOL msgAvailable = PeekMessage(&msg, NULL, 0xBECA, 0xBECA, PM_REMOVE);
        if (msgAvailable)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        if (lParam >> 16 == 0x6942)
        {
            unsigned int newLvlNum = ((lParam >> 6) & 0x3FF);
#ifdef DEBUG
            std::cout << "LM now in level " << std::hex << newLvlNum << std::endl;
#endif
            lvlNum = newLvlNum;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void handlePotentialROMChange()
{

}

#ifdef DEBUG
void AdjustConsoleBuffer(int16_t minLength)
{
    // Set the screen buffer to be big enough to scroll some text
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
    if (conInfo.dwSize.Y < minLength)
        conInfo.dwSize.Y = minLength;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);
}

bool CreateNewConsole(int16_t minLength)
{
    bool result = false;

    // Attempt to create new console
    if (AllocConsole())
    {
        AdjustConsoleBuffer(minLength);
        result = RedirectConsoleIO();
    }

    return result;
}

bool RedirectConsoleIO()
{
    bool result = true;
    FILE* fp;

    // Redirect STDIN if the console has an input handle
    if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
        if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
            result = false;
        else
            setvbuf(stdin, NULL, _IONBF, 0);

    // Redirect STDOUT if the console has an output handle
    if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
        if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
            result = false;
        else
            setvbuf(stdout, NULL, _IONBF, 0);

    // Redirect STDERR if the console has an error handle
    if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
        if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0)
            result = false;
        else
            setvbuf(stderr, NULL, _IONBF, 0);

    // Make C++ standard streams point to console as well.
    std::ios::sync_with_stdio(true);

    // Clear the error state for each of the C++ standard streams.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();

    return result;
}
#endif
