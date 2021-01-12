#include "math_statics.h"

Vector3 Vec3ToVector3(OpenSteer::Vec3 SteerVec)
{
	Vector3 result;

	result.x = SteerVec.x;
	result.y = SteerVec.y;
	result.z = SteerVec.z;

	return result;
}

OpenSteer::Vec3 Vector3ToVec3(Vector3 dxVector)
{
	OpenSteer::Vec3 result;

	result.x = dxVector.x;
	result.y = dxVector.y;
	result.z = dxVector.z;

	return result;
}

Vector3 GetRandomPointInSquare(Vector3 center, float SideLength)
{
	float randX = ((float)rand() / RAND_MAX) * SideLength;
	float randZ = ((float)rand() / RAND_MAX) * SideLength;

	center.x += (randX - SideLength / 2);
	center.z += (randZ - SideLength / 2);

	return center;
}