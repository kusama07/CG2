#pragma once
#include "Triangle.h"

class BlendMode
{public:
	void SettingBlend();


private:

	enum blendMode {
		//ブレンドなし
		kBlendModeNone,
		//通常αブレンド
		kBlendModeNormal,
		//加算
		kBlendaModeAdd,
		//減算
		kBlendModeSubtract,
		//乗算
		kBlendModeMultiply,
		//スクリーン
		kBlendModeScreen,
		//利用してはいけない
		kCountOfBlendMode,

	};

	D3D12_BLEND_DESC blendDesc{};

	

};