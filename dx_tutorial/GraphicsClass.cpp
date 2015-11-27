#include "GraphicsClass.h"



GraphicsClass::GraphicsClass()
{
	D3D = NULL;
	Camera = NULL;
	Model = NULL;
	TextureShader = NULL;
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

	Camera = new CameraClass();
	if (!Camera)
		return false;

	Camera->SetPosition(0.0f, 0.0f, -5.0f);

	Model = new ModelClass();
	if (!Model)
		return false;

	result = Model->Initialize(D3D->GetDevice(), L"../dx_tutorial/assets/rock.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model object", L"Error", MB_OK);
		return false;
	}

	TextureShader = new TextureShaderClass();
	if (!TextureShader)
		return false;

	result = TextureShader->Initialize(D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader object", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (TextureShader)
	{
		TextureShader->Shutdown();
		delete TextureShader;
		TextureShader = NULL;
	}

	if (Model)
	{
		Model->Shutdown();
		delete Model;
		Model = NULL;
	}

	if (Camera)
	{
		delete Camera;
		Camera = NULL;
	}

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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	
	D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	Camera->Render();

	D3D->GetWorldMatrix(worldMatrix);
	Camera->GetViewMatrix(viewMatrix);
	D3D->GetProjectionMatrix(projectionMatrix);

	Model->Render(D3D->GetDeviceContext());

	result = TextureShader->Render(D3D->GetDeviceContext(), Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, Model->GetTexture());
	if (!result)
		return false;

	D3D->EndScene();

	return true;
}
