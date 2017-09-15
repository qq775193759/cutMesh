#pragma once
#include "Geometry.h"

class RotateController
{
	float m[16], *externM;
	float width, height;
	Vector2D prevPoint2D;
	Vector3D prevPoint3D;

	void rotMatrix(Vector3D axis, float angle);
	void scaleMatrix(float scaleFactor);
	void multiply(float *left, float *right0, float *right1);
	Vector3D mapToSphere(Vector2D v2d);
public:
	RotateController(float* M);
	~RotateController(void);
	//reshape
	void resize(float w, float h){width = w/2.0; height = h/2.0;}
	//rotate
	void MouseMoveRotate(Vector2D v2d);
	void MousePress(Vector2D v2d);
	void doRot(Vector3D axis, float angle);
	void recoverRot();
	//scale
	void scale(float scaleFactor, Vector2D v2d);
	//shift
	void translate(Vector3D shift);
};

