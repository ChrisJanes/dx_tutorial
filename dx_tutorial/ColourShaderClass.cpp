#include "ColourShaderClass.h"



ColourShaderClass::ColourShaderClass()
{
	vertexShader = NULL;
	pixelShader = NULL;
	layout = NULL;
	matrixBuffer = NULL;
}

ColourShaderClass::ColourShaderClass(const ColourShaderClass &)
{
}

ColourShaderClass::~ColourShaderClass()
{
}

bool ColourShaderClass::Initialize(ID3D11Device *device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../dx_tutorial/shaders/ColourVertex.hlsl", L"../dx_tutorial/shaders/ColourPixel.hlsl");

	return result;
}

void ColourShaderClass::Shutdown()
{
	ShutdownShader();
}

bool ColourShaderClass::Render(ID3D11DeviceContext *deviceContext, int indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext,  worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
		return false;

	RenderShader(deviceContext, indexCount);

	return result;
}

bool ColourShaderClass::InitializeShader(ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	// compile the vertex shader
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else 
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// compile the pixel shader
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else 
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// create the shaders from the buffers.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// create the vertex input layout description
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);

	if (FAILED(result))
		return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;
	
	//set up the matrix constant buffer for the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColourShaderClass::ShutdownShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (layout)
	{
		layout->Release();
		layout = NULL;
	}

	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = NULL;
	}

	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = NULL;
	}
}

void ColourShaderClass::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, WCHAR *shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	ofstream fout;

	// get a pointer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// get the length of the error
	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (unsigned long i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = NULL;

	MessageBox(hwnd, L"Error compiling shader, check shader-error.text for details", shaderFilename, MB_OK);
}

bool ColourShaderClass::SetShaderParameters(ID3D11DeviceContext *deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// lock the matrix buffer
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// copy the matrices into the buffer
	dataPtr->world = XMMatrixTranspose(worldMatrix);
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projectionMatrix);

	// unlock the buffer
	deviceContext->Unmap(matrixBuffer, 0);

	// set the buffer in the vertex shader
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	return true;
}

void ColourShaderClass::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(layout);

	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
