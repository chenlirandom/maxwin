#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <vector>

#define LogInfo(format, ...) fprintf(stdout, format, __VA_ARGS__)
#define LogError(format, ...) fprintf(stderr, format, __VA_ARGS__)

void AdjustAspectRatio(int & w, int & h, const int sw, const int sh)
{
	if (w * sh > h * sw)
	{
		// too wide, narrow it
		w = h * sw / sh;
	}
	else
	{
		h = w * sh / sw;
	}
};

// Make windows covers the whole desktop but not a taskbar
int MakeFullDesktop(HWND window)
{
	if (!IsWindow(window))
	{
		LogError("0x%X is not a valid window handle", (int)window);
		return -1;
	}

	// get current display resolution
	HMONITOR hmon = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi))
	{
		LogError("GetMonitorInfo() failed");
		return -1;
	}
	int x = mi.rcWork.left;
	int y = mi.rcWork.top;
	int w = mi.rcWork.right - mi.rcWork.left;
	int h = mi.rcWork.bottom - mi.rcWork.top;
	// adjust aspect ratio to match full screen ratio.
	AdjustAspectRatio(w, h, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);

	// switch it to norml state, in case it is maximized.
	ShowWindow(window, SW_SHOWNORMAL);

	// update window style to borderless
	if (!SetWindowLongPtr(window, GWL_STYLE, WS_POPUP | WS_VISIBLE))
	{
		int error = GetLastError();
		LogError("SetWindowLongPtr(GWL_STYLE, ...) failed. Error=0x%X", error);
		return -1;
	}

	// set size and position of the window (make it top most, so it covers the taskbar too)
	SetWindowPos(window, HWND_TOP, x, y, w, h, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	UpdateWindow(window);

	// done
	return 0;
}

// Make the window covers the full screen including task bar.
int MakeFullscreen(HWND window)
{
	if (!IsWindow(window))
	{
		LogError("0x%X is not a valid window handle", (int)window);
		return -1;
	}

	// get current display resolution
	HMONITOR hmon = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi))
	{
		LogError("GetMonitorInfo() failed");
		return -1;
	}
	int x = mi.rcMonitor.left;
	int y = mi.rcMonitor.top;
	int w = mi.rcMonitor.right - mi.rcMonitor.left;
	int h = mi.rcMonitor.bottom - mi.rcMonitor.top;

	// switch it to norml state, in case it is maximized.
	ShowWindow(window, SW_SHOWNORMAL);

	// update window style to borderless
	if (!SetWindowLongPtr(window, GWL_STYLE, WS_POPUP | WS_VISIBLE))
	{
		int error = GetLastError();
		LogError("SetWindowLongPtr(GWL_STYLE, ...) failed. Error=0x%X", error);
		return -1;
	}

	// set size and position of the window (make it top most, so it covers the taskbar too)
	SetWindowPos(window, HWND_TOP, x, y, w, h, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	UpdateWindow(window);

	// done
	return 0;
}

// Maximize the window (not changing any other styles)
int MaxIt(HWND window)
{
    if (!IsWindow(window))
    {
        LogError("0x%X is not a valid window handle", (int)window);
        return -1;
    }

    int current = GetWindowLongPtr(window, GWL_STYLE);

	int newStyle = current | WS_MAXIMIZEBOX | WS_SIZEBOX;

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
	bool borderless = false;
    if (0 == _strcmpi("357", a1))
    {
        // find main window of 357 (launch.exe)
        targetWindow = FindWindowW(L"SM7EX_WIN_TC", nullptr);
        if (0 == targetWindow)
        {
            LogError("Can't find 357's main window.");
			return -1;
		}
		return MaxIt(targetWindow);
    }
	else if (0 == _strcmpi("sle", a1))
	{
		// find main window of skyrim legendary edition
		targetWindow = FindWindowW(L"Skyrim", nullptr);
		if (0 == targetWindow)
		{
			LogError("Can't find Skyrim Special Edition main window.");
			return -1;
		}
		return MakeFullscreen(targetWindow);
	}
	else if (0 == _strcmpi("sse", a1))
	{
		// find main window of skyrim special edition
		targetWindow = FindWindowW(L"Skyrim Special Edition", L"Skyrim Special Edition");
		if (0 == targetWindow)
		{
			LogError("Can't find Skyrim Special Edition main window.");
			return -1;
		}
		return MakeFullscreen(targetWindow);
	}
	else if (0 == _strcmpi("2b", a1))
	{
		// find main window of Nier: Automata
		targetWindow = FindWindowW(L"NieR:Automata_MainWindow", L"NieR:Automata");
		if (0 == targetWindow)
		{
			LogError("Can't find NieR:Automata's main window.");
			return -1;
		}
		return MakeFullDesktop(targetWindow);
	}
	else if (0 == _strcmpi("2b-fs", a1))
	{
		// find main window of Nier: Automata
		targetWindow = FindWindowW(L"NieR:Automata_MainWindow", L"NieR:Automata");
		if (0 == targetWindow)
		{
			LogError("Can't find NieR:Automata's main window.");
			return -1;
		}
		return MakeFullscreen(targetWindow);
	}
	else if (0 == _strcmpi("bdo", a1))
	{
		// find main window of Nier: Automata
		targetWindow = FindWindowW(L"BlackDesertWindowClass", nullptr);
		if (0 == targetWindow)
		{
			LogError("Can't find Blade Desert Online main window.");
			return -1;
		}
		return MaxIt(targetWindow);
	}
    else if (0 == _strcmpi("unity", a1))
    {
        // find main window of unity based game.
        targetWindow = FindWindowW(L"UnityWndClass", nullptr);
        if (0 == targetWindow)
        {
            LogError("Can't find Unity main window.");
            return -1;
        }
        return MakeFullscreen(targetWindow);
    }
    else
    {
        targetWindow = (HWND)strtol(a1, nullptr, 16);
        if (0 == targetWindow)
        {
            LogError("%s is not a valid hex number.", a1);
			return -1;
		}
		return MaxIt(targetWindow);
    }
}