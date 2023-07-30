#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <assert.h>
#include <cmath>

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

//X軸回転行列
Matrix4x4 MakeRotateXmatrix(float radian);

//Y軸回転行列
Matrix4x4 MakeRotateYmatrix(float radian);

//Z軸回転行列
Matrix4x4 MakeRotateZmatrix(float radian);

//平行移動
Matrix4x4 MakeTranslateMatrix(Vector3 translate);

//拡大縮小
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の減法
Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m1);

//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);

//単位行列
Matrix4x4 MakeIdentity4x4();

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRadio, float nearClip, float farClip);
