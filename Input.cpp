#include "Input.h"

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	// DirectInputの初期化

	result_ = DirectInput8Create(
		winApp_->GetHInst(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput_,
		nullptr
	);

	assert(SUCCEEDED(result_));

	// キーボードデバイスの生成
	result_ = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result_));

	// 入力データ形式のセット
	result_ = keyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result_));

	// 排他的制御レベルセット
	result_ = keyboard_->SetCooperativeLevel(
		winApp_->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result_));

}

void Input::Updata()
{
	// 前回のキー入力を保存
	memcpy(preKeys_, keys_, sizeof(keys_));

	keyboard_->Acquire();
	//全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(keys_), keys_);
}

Input* Input::GetInstance()
{
	static Input instance;
	return &instance;
}

bool Input::NoneKey(BYTE keyNumber)
{
	if (!keys_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::ReleassKey(BYTE keyNumber)
{
	if (preKeys_[keyNumber] && keys_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::PushKey(BYTE keyNumber)
{
	// 指定キーを押していればtrueを返す
	if (keys_[keyNumber]) {
		return true;
	}
	// そうでなければfalseを渡す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (!preKeys_[keyNumber] && keys_[keyNumber]) {
		return true;
	}

	return false;
}
