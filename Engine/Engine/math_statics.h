#pragma once

#include "SimpleMath.h"
#include "AISystem/SteerLibrary.h"
#include <random>

using namespace DirectX::SimpleMath;

Vector3 Vec3ToVector3(OpenSteer::Vec3 SteerVec);

OpenSteer::Vec3 Vector3ToVec3(Vector3 dxVector);

Vector3 GetRandomPointInSquare(Vector3 center, float SideLength);
