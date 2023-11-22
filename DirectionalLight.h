#pragma once
#include "math/Vector4.h"
#include "math/Vector3.h"

struct DirectionlLight {
	Vector4 color; //!< ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity; //!< 輝度
};