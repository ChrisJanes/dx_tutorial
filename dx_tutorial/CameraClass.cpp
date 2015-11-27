#include "CameraClass.h"

CameraClass::CameraClass()
{
	positionX = 0.0f;
	positionY = 0.0f;
	positionZ = 0.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass &)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	positionX = x;
	positionY = y;
	positionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	rotationX = x;
	rotationY = y;
	rotationZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(positionX, positionY, positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// setup up vector
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	position.x = positionX;
	position.y = positionY;
	position.z = positionZ;

	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	// set the yaw, pitch and roll in radians.
	pitch = rotationX * 0.0174532925f;
	yaw = rotationY * 0.0174532925f;
	roll = rotationZ * 0.0174532925f;
	
	// build the rotation matrix
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// transform the lookat and up vectors by the rotation matrix
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// translate the camera to the look at position
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// create the view matrix
	viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
}
