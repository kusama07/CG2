#include "Sprite.h"
#include "Triangle.h"
#include "Mesh.h"

const wchar_t kWindowTitle[] = { L"CG2" };

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WinApp* win_ = nullptr;
	Sprite* sprite = new Sprite;
	Mesh* mesh = new Mesh;

	sprite->Initialization(win_, kWindowTitle, 1280, 720);

	sprite->Initialize();
	mesh->Initialize();

	Vector4 data[15];

	data[0] = { 0.4f,0.4f,0.0f,1.0f };
	data[1] = { 0.6f,0.8f,0.0f,1.0f };
	data[2] = { 0.8f,0.4f,0.0f,1.0f };
	data[3] = { -0.8f,-0.8f,0.0f,1.0f };
	data[4] = { -0.6f,-0.4f,0.0f,1.0f };
	data[5] = { -0.4f,-0.8f,0.0f,1.0f };
	data[6] = { 0.4f,-0.8f,0.0f,1.0f };
	data[7] = { 0.6f,-0.4f,0.0f,1.0f };
	data[8] = { 0.8f,-0.8f,0.0f,1.0f };
	data[9] = { -0.8f,0.4f,0.0f,1.0f };
	data[10] = { -0.6f,0.8f,0.0f,1.0f };
	data[11] = { -0.4f,0.4f,0.0f,1.0f };
	data[12] = { -0.2f,-0.2f,0.0f,1.0f };
	data[13] = { 0.0f,0.2f,0.0f,1.0f };
	data[14] = { 0.2f,-0.2f,0.0f,1.0f };

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

			for (int i = 0; i < 5; i++) {
				// 三角形の描画
				mesh->Draw(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

			}

			sprite->EndFrame();
		}
	}

	sprite->Finalize();

	return 0;
}