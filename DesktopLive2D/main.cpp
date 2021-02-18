#include <windows.h>
#include <iostream>

#include "App.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int) {
	OutputDebugString(L"Initialize application...\n");
	
	App app(instance);
	BOOL result = app.Initialize();
	
	if (!result) {
		OutputDebugString(L"Failed to initalize application, releasing...\n");
		app.Release();
		return result;
	}

	OutputDebugString(L"Application initalized\n");

	app.Run();

	return 0;
}