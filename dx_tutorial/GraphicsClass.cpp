#include "GraphicsClass.h"



GraphicsClass::GraphicsClass()
{
	D3D = NULL;
}


GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// create the D3D object
	D3D = new D3DClass();
	if (!D3D)
		return false;

	result = D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Cound not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (D3D)
	{
		D3D->Shutdown();
		delete D3D;
		D3D = NULL;
	}
}

bool GraphicsClass::Frame()
{
	bool result;

	result = Render();

	return result;
}

bool GraphicsClass::Render()
{
	D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	D3D->EndScene();

	return true;
}
