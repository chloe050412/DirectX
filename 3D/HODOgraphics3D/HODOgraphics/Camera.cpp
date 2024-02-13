#include "Camera.h"

Camera::Camera()
{
	_position.x = 0;
	_position.y = 0;
	_position.z = 0;

	_rotation.x = 0;
	_rotation.y = 0;
	_rotation.z = 0;
}

Camera::Camera(const Camera&)
{
	_position.x = 0;
	_position.y = 0;
	_position.z = 0;

	_rotation.x = 0;
	_rotation.y = 0;
	_rotation.z = 0;
}

Camera::~Camera()
{

}

void Camera::SetPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	_rotation.x = x;
	_rotation.y = y;
	_rotation.z = z;
}

Vector3 Camera::GetPosition()
{
	return _position;
}

Vector3 Camera::GetRotation()
{
	return _rotation;
}

void Camera::Render()
{
	Vector3 up;
	Vector3 position;
	Vector3 lookAt;

	float yaw, pitch, roll;

	Matrix rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position = _position;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = _rotation.x * 0.0174532925f;
	yaw = _rotation.y * 0.0174532925f;
	roll = _rotation.z * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix.CreateFromYawPitchRoll(yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = XMVectorAdd(position, lookAt);

	// Finally create the view matrix from the three updated vectors.
	_viewMatrix = XMMatrixLookAtLH(_position, lookAt, up);
}

void Camera::GetViewMatrix(Matrix& mat)
{
	mat = _viewMatrix;
}