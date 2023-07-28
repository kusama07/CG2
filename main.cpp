#include<Windows.h>
#include "MyEngine.h"
#include "Triangle.h"
#include "WinApp.h"
#include "DirectXCommon.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WinApp* winApp = new WinApp;
	DirectXCommon* dxCommon = new DirectXCommon;
	MyEngine* myEngine = new MyEngine;

	// ゲームシーンの初期化
	winApp->StartApp();
	dxCommon->Initialize(winApp->GetHwnd());
	myEngine->Initialize();

	Vector4 triangleData[10][3];
	Triangle* triangle[4];

	for (int i = 0; i < 4; i++)
	{
		triangleData[i][0] = { -0.2f + (i * 0.4f),-0.8f,0.0f,1.5f };
		triangleData[i][1] = { 0.0f + (i * 0.4f),-0.4f,0.0f,1.5f };
		triangleData[i][2] = { 0.2f + (i * 0.4f),-0.8f,0.0f,1.5f };
		triangle[i] = new Triangle;
		triangle[i]->Initialize(dxCommon, triangleData[i][0], triangleData[i][1], triangleData[i][2]);
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
			myEngine->Update();

			for (int i = 0; i < 4; i++) {
				triangle[i]->Draw();
			}
			myEngine->UpdateEnd();
		}
	}

	for (int i = 0; i < 4; i++) {
		triangle[i]->Finalize();
	}
	winApp->EndApp();
	myEngine->Finalize();
	dxCommon->PostDraw();

	return 0;
}