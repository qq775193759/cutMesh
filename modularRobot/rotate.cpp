#include "rotate.h"
#include <cmath>
#include <istream>


RotateController::RotateController(float* M)
{
	externM = M;
	width=height=320;
}


RotateController::~RotateController(void)
{
}

void RotateController::rotMatrix(Vector3D axis, float angle)
{
	float c = cos(angle), s = sin(angle);
	memset(m, 0, 16 * sizeof(float));
	m[0] = c + (1 - c)*axis.x*axis.x;
	m[1] = (1 - c)*axis.y*axis.x + s*axis.z;
	m[2] = (1 - c)*axis.z*axis.x - s*axis.y;
	m[4] = (1 - c)*axis.x*axis.y - s*axis.z;
	m[5] = c + (1 - c)*axis.y*axis.y;
	m[6] = (1 - c)*axis.z*axis.y + s*axis.x;
	m[8] = (1 - c)*axis.x*axis.z + s*axis.y;
	m[9] = (1 - c)*axis.y*axis.z - s*axis.x;
	m[10] = c + (1 - c)*axis.z*axis.z;
	m[15] = 1.0;
}

void RotateController::multiply(float *left, float *right0, float *right1)
{
	float res[16];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			res[j * 4 + i] = 0.0;
			for (int k = 0; k < 4; ++k)
				res[j * 4 + i] += right0[k * 4 + i] * right1[j * 4 + k];
		}
	}
	for (int i = 0; i < 16; ++i) left[i] = res[i];
}

void RotateController::doRot(Vector3D axis, float angle)
{
	Vector3D shift;
	for (int i = 0; i < 3; ++i)
	{
		shift[i] = externM[12 + i];
		externM[12 + i] = 0.0;
	}
	rotMatrix(axis, angle);
	multiply(externM, m, externM);
	for (int i = 0; i < 3; ++i)
		externM[12 + i] = shift[i];
}

Vector3D RotateController::mapToSphere(Vector2D v2d)
{
	Vector3D v3d;
	v3d.x = (v2d.x - width) / width;
	v3d.y = (height - v2d.y) / height;
	float v3dLen = v3d.length();
	if (v3dLen < 1.0)
		v3d.z = sqrt(1.0 - v3dLen*v3dLen);
	else
		v3d /= v3dLen;
	return v3d;
}

void RotateController::MouseMoveRotate(Vector2D v2d)
{
	Vector3D currentPoint3D = mapToSphere(v2d);
	Vector3D axis = prevPoint3D ^ currentPoint3D;
	if (axis.length() < 1e-7) return;
	axis.normalize();
	double acosAngle = prevPoint3D * currentPoint3D;
	if (acosAngle > 1.0) acosAngle = 1.0;
	else if (acosAngle < -1.0) acosAngle = -1.0;
	float angle = acos(acosAngle);
	doRot(axis, 2.0 * angle);
	prevPoint3D = currentPoint3D;
}

void RotateController::MousePress(Vector2D v2d)
{
	prevPoint3D = mapToSphere(v2d);
}

void RotateController::scaleMatrix(float scaleFactor)
{
	memset(m, 0, 16 * sizeof(float));
	m[0] = scaleFactor; m[5] = scaleFactor; m[10] = scaleFactor;
	m[15] = 1.0;
}

void RotateController::scale(float scaleFactor, Vector2D v2d)
{
	/*Vector3D zoomCenter;
	for (int i = 0; i < 3; ++i)
		externM[12 + i] -= zoomCenter[i];*/
	scaleMatrix(scaleFactor);
	multiply(externM, m, externM);
	/*for (int i = 0; i < 3; ++i)
		externM[12 + i] += zoomCenter[i];*/
}

void RotateController::translate(Vector3D shift)
{
	for (int i = 0; i < 3; ++i)
		externM[12 + i] += shift[i];
}

