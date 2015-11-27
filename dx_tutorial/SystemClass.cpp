#include "SystemClass.h"



SystemClass::SystemClass()
{
	input = NULL;
	graphics = NULL;
}

SystemClass::SystemClass(const SystemClass &)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// initialize screen dimension variables to 0.
	screenWidth = screenHeight = 0;

	// initialize the windows API - pass in references to screen dimension variables to get them set.
	InitializeWindow(screenWidth, screenHeight);

	input = new InputClass();
	if (!input)
		return false;

	input->Initialize();

	graphics = new GraphicsClass();

	if (!graphics)
		return false;

	result = graphics->Initialize(screenWidth, screenHeight, hWnd);
	if (!result)
		return false;

	return result;
}

void SystemClass::Shutdown()
{
	// release graphics object
	if (graphics)
	{
		graphics->Shutdown();
		delete graphics;
		graphics = NULL;
	}

	if (input)
	{
		delete input;
		input = NULL;
	}

	ShutdownWindows();
}

// run is our game loop - so this will keep calling itself forever until the user quits.
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	// enter an infinite loop until the window or user says to quit
	done = false;
	while (!done)
	{
		// handle the window messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// check for the quit message
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else {
			// handle the current frame
			result = Frame();
			if (!result)
				done = true;
		}
	}
}

bool SystemClass::Frame()
{
	bool result;

	if (input->IsKeyDown(VK_ESCAPE))
		return false;

	result = graphics->Frame();
	if (!result)
		return false;

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_KEYDOWN:
		{
			input->KeyDown((unsigned int)wparam);
			return 0;
		}
		case WM_KEYUP:
		{
			input->KeyUp((unsigned int)wparam);
			return 0;
		}
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindow(int& screenWidth, int& screenHeight)
{
	// build the window to actually draw things to.
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// get an external pointer to the object
	ApplicationHandle = this;

	// get the instance of the application;
	hInstance = GetModuleHandle(NULL);

	//set a name
	applicationName = L"Engine";

	// set the window class values
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = applicationName;
	wc.lpszMenuName = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);

	// register the window class
	RegisterClassEx(&wc);

	// find the desktop resolution
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		// set the screen to the max size of the user desktop and enforce 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// make the changes actually happen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// set the window position to the top left corner
		posX = posY = 0;
	}
	else {
		// otherwise, we'll work at 800x600
		screenWidth = 800;
		screenHeight = 600;

		// and position the window at the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// create the window using the above settings.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
	 	WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

	// bring the window up and give it focus
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// remove the window
	DestroyWindow(hWnd);
	hWnd = NULL;

	// remove the application instance
	UnregisterClass(applicationName, hInstance);
	hInstance = NULL;

	// release the pointer
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}