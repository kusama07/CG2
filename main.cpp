#include "Sprite.h"
#include "Triangle.h"

const wchar_t kWindowTitle[] = { L"CG2" };

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WindowsClass* win_ = nullptr;
	Sprite* sprite = new Sprite;
	sprite->Initialization(win_, kWindowTitle, 1280, 720);

	sprite->Initialize();

	Vector4 data1 = { 0.4f,0.4f,0.0f,1.0f };
	Vector4 data2 = { 0.6f,0.8f,0.0f,1.0f };
	Vector4 data3 = { 0.8f,0.4f,0.0f,1.0f };

	Vector4 data4 = { -0.8f,-0.8f,0.0f,1.0f };
	Vector4 data5 = { -0.6f,-0.4f,0.0f,1.0f };
	Vector4 data6 = { -0.4f,-0.8f,0.0f,1.0f };

	Vector4 data7 = { 0.4f,-0.8f,0.0f,1.0f };
	Vector4 data8 = { 0.6f,-0.4f,0.0f,1.0f };
	Vector4 data9 = { 0.8f,-0.8f,0.0f,1.0f };

	Vector4 data10 = { -0.8f,0.4f,0.0f,1.0f };
	Vector4 data11 = { -0.6f,0.8f,0.0f,1.0f };
	Vector4 data12 = { -0.4f,0.4f,0.0f,1.0f };

	Vector4 data13 = { -0.2f,-0.2f,0.0f,1.0f };
	Vector4 data14 = { 0.0f,0.2f,0.0f,1.0f };
	Vector4 data15 = { 0.2f,-0.2f,0.0f,1.0f };

	MSG msg{};

	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT) {
		//windowのメッセージを最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//ゲームの処理
			sprite->Update();
			sprite->BeginFrame();


			//三角形描画
			sprite->DrawTriangle(data1, data2, data3);

			sprite->DrawTriangle(data4, data5, data6);

			sprite->DrawTriangle(data7, data8, data9);

			sprite->DrawTriangle(data10, data11, data12);

			sprite->DrawTriangle(data13, data14, data15);

			sprite->EndFrame();
		}
	}

	sprite->Finalize();

	return 0;
}