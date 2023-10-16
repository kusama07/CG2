#pragma once
#include "Matrix4x4.h" 
#include "MyMath.h"

class Camera
{
public:
	void Initialize(uint32_t kClientWidth, uint32_t kClientHeight);

	void Updata();

	Matrix4x4 transformMatrix_;

private:
	uint32_t kClientWidth_;

	uint32_t kClientHeight_;

	Transform cameraTransform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,-7.0f},
	};

	Transform transform_{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	Matrix4x4 worldMatrix_;
	
	Matrix4x4 viewMatrix_;
	
	Matrix4x4 cameraMatrix_;

	Matrix4x4 projectionMatrix_;

	Matrix4x4 worldViewProjectionMatrix_;

};

