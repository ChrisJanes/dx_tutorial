#include "ModelClass.h"



ModelClass::ModelClass()
{
	vertexBuffer = NULL;
	indexBuffer = NULL;
}

ModelClass::ModelClass(const ModelClass &)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device * device)
{
	bool result;

	result = InitializeBuffers(device);
	
	return result;
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext * deviceContext)
{
	RenderBuffers(deviceContext);
	
}

int ModelClass::GetIndexCount()
{
	return indexCount;
}

// usually this would read in and load a model file, but this is just going to hard code a triangle.
bool ModelClass::InitializeBuffers(ID3D11Device * device)
{
	VertexType *vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertexCount = 3;
	indexCount = 3;
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// load the vertex array with data
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // bottom left
	vertices[0].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // top middle
	vertices[1].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // bottom right
	vertices[2].colour = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// load the vertices into the index array
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// build the vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// point the subresource structure to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// set up the description of the index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// tidy up
	delete[] vertices;
	vertices = NULL;

	delete[] indices;
	indices = NULL;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = NULL;
	}

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride, offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
