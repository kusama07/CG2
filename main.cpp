#include<Windows.h>
#include "Sprite.h"
#include "Triangle.h"
#include "WinApp.h"
#include "DirectXCommon.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WinApp* winApp = new WinApp;
	DirectXCommon* dxCommon = new DirectXCommon;
	Sprite* sprite = new Sprite;

	// ゲームシーンの初期化
	winApp->StartApp();
	dxCommon->Initialize(winApp->GetHwnd());
	sprite->Initialize();

	Vector4 triangleData[10][3];
	Triangle* triangle[4];
	Vector4 Material[4];

	for (int i = 0; i < 4; i++)
	{
		triangleData[i][0] = { -0.2f + (i * 0.4f),-0.8f,0.0f,1.5f };
		triangleData[i][1] = { 0.0f + (i * 0.4f),-0.4f,0.0f,1.5f };
		triangleData[i][2] = { 0.2f + (i * 0.4f),-0.8f,0.0f,1.5f };
		Material[i] = { 1.0f,0.1f * i,0.2f * i,1.0f };
		triangle[i] = new Triangle;
		triangle[i]->Initialize(dxCommon, triangleData[i][0], triangleData[i][1], triangleData[i][2],Material[i]);
	}

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

			for (int i = 0; i < 4; i++) {
				triangle[i]->Draw();
			}
			sprite->UpdateEnd();
		}
	}

	for (int i = 0; i < 4; i++) {
		triangle[i]->Finalize();
	}
	winApp->EndApp();
	sprite->FInalize();
	dxCommon->PostDraw();

	return 0;
}