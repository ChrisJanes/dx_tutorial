#include "D3DClass.h"



D3DClass::D3DClass()
{
	swapChain = NULL;
	device = NULL;
	deviceContext = NULL;
	renderTargetView = NULL;
	depthStencilBuffer = NULL;
	depthStencilState = NULL;
	depthStencilView = NULL;
	rasterState = NULL;
}

D3DClass::D3DClass(const D3DClass &)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator, stringLength;
	DXGI_ADAPTER_DESC adapterDescription;
	DXGI_MODE_DESC* displayModeList;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPointer;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	vsyncEnabled = vsync;

	// create a graphics interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// use the factory to create an adapter
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// enumerate the primary output
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// get the modes that match DXGI_FORMAT_R8G8B8A8_UNORM for the output
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// build a list of all possible display modes for the output
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// fill the list of display modes
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// find the display mode that matches the current screen width and height.
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// now grab the name of the video card and the amount of ram it has.
	result = adapter->GetDesc(&adapterDescription);
	if (FAILED(result))
	{
		return false;
	}

	videoCardMemory = (int)(adapterDescription.DedicatedVideoMemory / 1024 / 1024);

	// convert the video card name into a char array
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDescription.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// tidy up after ourselves.
	delete[] displayModeList;
	displayModeList = NULL;

	adapterOutput->Release();
	adapterOutput = NULL;

	adapter->Release();
	adapter = NULL;

	factory->Release();
	factory = NULL;

	// set up the swap chain for the double buffering

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// set to a single back buffer
	swapChainDesc.BufferCount = 1;

	// set the width and height of the buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// set a 32 bit surface for the buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// set up the refresh rate
	if (vsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	}

	// set the usage of the buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = hwnd;

	// turn off multisampling
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	// set the scan line ordering
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// discard the back buffer contents after display
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// create the swap chain, d3d device and context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION,
		&swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// attach the back buffer to the swap chain
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPointer);
	if (FAILED(result))
	{
		return false;
	}

	// create the render target view
	result = device->CreateRenderTargetView(backBufferPointer, NULL, &renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// release the pointer to the back buffer
	backBufferPointer->Release();
	backBufferPointer = NULL;

	// set up the depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// setup the descriptions of the back buffer
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// create the depth buffer texture
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// set up the depth stencil buffer
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// set up the stencil buffer description
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// if pixel is front facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// create the depth stencil state
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// set the depth stencil state
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// setup the depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// bind the render target view and the depth stencil buffer to the output render pipeline
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// for learning purposes, we'll setup a rasterizer state here now too - we could just use the DX default.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		return false;
	}

	deviceContext->RSSetState(rasterState);

	//setup the viewport
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);

	// create the projection matrix
	fieldOfView = XM_PIDIV4;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// create the matrix
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// create the world matrix
	worldMatrix = XMMatrixIdentity();

	// create the ortho matrix
	orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// we need to return to windowed mode before we shut down to avoid an error being thrown.
	if (swapChain)
		swapChain->SetFullscreenState(false, NULL);

	if (rasterState)
	{
		rasterState->Release();
		rasterState = NULL;
	}

	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = NULL;
	}

	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = NULL;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = NULL;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = NULL;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = NULL;
	}

	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (swapChain)
	{
		swapChain->Release();
		swapChain = NULL;
	}
}

// sets up our buffers so they are ready to be drawn to
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// clear the back buffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);

	// clear the depth buffer
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	// swap the back buffer to the screen
	if (vsyncEnabled)
	{
		swapChain->Present(1, 0);
	}
	else 
	{
		swapChain->Present(0, 0);
	}
}

ID3D11Device * D3DClass::GetDevice()
{
	return device;
}

ID3D11DeviceContext * D3DClass::GetDeviceContext()
{
	return deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX &projMatrix)
{
	projMatrix = projectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX &worlMatrix)
{
	worlMatrix = worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX &orthMatrix)
{
	orthMatrix = orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char *cardName, int &memory)
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
}
