#include <stdio.h>
#include <windows.h>

int MakeItMaximizable(HWND window)
{
    if (!IsWindow(window))
    {
        fprintf(stderr, "0x%X is not a valid window handle", (int)window);
        return -1;
    }

    int current = GetWindowLongPtr(window, GWL_STYLE);
    if (WS_MAXIMIZEBOX & current)
    {
        fprintf(stdout, "Windows 0x%X is already maximizable.", (int)window);
        return 0;
    }

    int newStyle = current | WS_MAXIMIZEBOX;
    if (0 == SetWindowLongPtr(window, GWL_STYLE, newStyle))
    {
        int error = GetLastError();
        fprintf(stderr, "SetWindowLongPtr failed. Error=0x%X", error);
        return -1;
    }

    return 0;
}

int main(int argc, const char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "no parameter specified.");
        return -1;
    }

    // Determine the window handle based on argv[1]
    const char * a1 = argv[1];
    HWND targetWindow = 0;
    if (0 == _strcmpi("357", a1))
    {
        // find main window of 357 (launch.exe)
        targetWindow = FindWindowW(L"SM7EX_WIN_TC", nullptr);
        if (0 == targetWindow)
        {
            fwprintf(stderr, L"Can't find 357's main window");
        }
    }
    else
    {
        targetWindow = (HWND)strtol(a1, nullptr, 16);
        if (0 == targetWindow)
        {
            fprintf(stderr, "%s is not a valid hex number.", a1);
        }
    }
    if (0 == targetWindow)
    {
        return -1;
    }

    return MakeItMaximizable(targetWindow);
}