#pragma once
class windowsClass
{
public:

	static const int kClientWidth = 1280;
	static const int kClientHeight = 720;

	void Initialize();

	void WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:

};

