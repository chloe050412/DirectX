#pragma once
#include "HDmath.h"

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	Vector3 GetPosition();
	Vector3 GetRotation();

	void Render();
	void GetViewMatrix(Matrix&);

private:
	Vector3 _position;
	Vector3 _rotation;

	Matrix _viewMatrix;
};

