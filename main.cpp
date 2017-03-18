#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <vector>

#define LogInfo(format, ...) fprintf(stdout, format, __VA_ARGS__)
#define LogError(format, ...) fprintf(stderr, format, __VA_ARGS__)

enum MaxOptions : int
{
	BORDERLESS = 1,
};

int MaxIt(HWND window, int options = 0)
{
    if (!IsWindow(window))
    {
        LogError("0x%X is not a valid window handle", (int)window);
        return -1;
    }

    int current = GetWindowLongPtr(window, GWL_STYLE);

	int newStyle = current;

	// make it maximizable
	if (0 == (WS_MAXIMIZEBOX & current))
	{
		newStyle |= WS_MAXIMIZEBOX;
	}

	// make it borderless
	if (options & BORDERLESS)
	{
		newStyle &= ~WS_BORDER;
		newStyle &= ~WS_CAPTION;
	}

	// update window style
	if (newStyle != current && 0 == SetWindowLongPtr(window, GWL_STYLE, newStyle))
    {
        int error = GetLastError();
        LogError("SetWindowLongPtr failed. Error=0x%X", error);
        return -1;
    }

	// switch it to norml state, first. In case that the window is already maximized;
	ShowWindow(window, SW_SHOWNORMAL);

	// then max it
	ShowWindow(window, SW_SHOWMAXIMIZED);

    return 0;
}

/*HWND FindWindow(const char * exename)
{
	std::vector<DWORD> processes(256);
	DWORD cb, bytesReturned;
	do
	{
		cb = processes.size() * sizeof(DWORD);
		if (!EnumProcesses(processes.data(), cb, &bytesReturned))
		{
			LogError("EnumProcesses() failed.");
			return 0;
		}
	} while (bytesReturned == cb);

	DWORD count = bytesReturned / sizeof(DWORD);

	for (DWORD i = 0; i < count; ++i)
	{
		HANDLE processHandle = 0;
		__try
		{
			processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processes[i]);
			char basename[256];
			if (GetModuleBaseNameA(processHandle, NULL, basename, sizeof(basename)))
			{
				if (0 == _strcmpi(basename, exename))
				{
					// found it!
					*pWindow = hwnd;
					return FALSE;
				}
			}
		}
		__finally
		{
			if (processHandle) CloseHandle(processHandle);
		}
	}
}*/

int main(int argc, const char * argv[])
{
    if (argc < 2)
    {
        LogError("no parameter specified.");
        return -1;
    }

    // Determine the window handle based on argv[1]
    const char * a1 = argv[1];
    HWND targetWindow = 0;
	int options = 0;
    if (0 == _strcmpi("357", a1))
    {
        // find main window of 357 (launch.exe)
        targetWindow = FindWindowW(L"SM7EX_WIN_TC", nullptr);
        if (0 == targetWindow)
        {
            LogError("Can't find 357's main window.");
        }
    }
	else if (0 == _strcmpi("2b", a1))
	{
		// find main window of Nier: Automata
		targetWindow = FindWindowW(L"NieR:Automata_MainWindow", L"NieR:Automata");
		if (0 == targetWindow)
		{
			LogError("Can't find NieR:Automata's main window.");
		}
		options = BORDERLESS;
	}
    else
    {
        targetWindow = (HWND)strtol(a1, nullptr, 16);
        if (0 == targetWindow)
        {
            LogError("%s is not a valid hex number.", a1);
        }
    }
    if (0 == targetWindow)
    {
        return -1;
    }

    return MaxIt(targetWindow, options);
}