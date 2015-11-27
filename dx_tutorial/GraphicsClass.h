#pragma once

#include <Windows.h>
#include "d3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColourShaderClass.h"


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.01f;

class GraphicsClass
{
public:
	GraphicsClass();
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

	D3DClass *D3D;
	CameraClass* Camera;
	ModelClass* Model;
	ColourShaderClass* ColourShader;

};

