#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}

private:
	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;
	XMMATRIX viewMatrix;
};

