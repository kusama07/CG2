#include "Sprite.h"
#include "Triangle.h"
#include "GameScene.h"

const wchar_t kWindowTitle[] = { L"CG2" };

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WinApp* win_ = nullptr;
	Sprite* sprite = new Sprite;
	Mesh* mesh = new Mesh;
	GameScene* gameScene = nullptr;

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	sprite->Initialization(win_, kWindowTitle, 1280, 720);

	sprite->Initialize();

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

			gameScene->Draw();

			sprite->EndFrame();
		}
	}

	sprite->Finalize();

	return 0;
}