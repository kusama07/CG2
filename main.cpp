#include<Windows.h>
#include "MyEngine.h"
#include "Triangle.h"
#include "WinApp.h"
#include "DirectXCommon.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* winApp = new WinApp;
	DirectXCommon* dxCommon = new DirectXCommon;
	MyEngine* myEngine = new MyEngine;

	// ゲームシーンの初期化
	winApp->StartApp();
	dxCommon->Initialize(winApp->GetHwnd());
	myEngine->Initialize();

	Vector4 triangleData[10][3];
	Triangle* triangle[4];
	Vector4 Material[4] = {};
	float materialcolor[3] = { Material[0].x ,Material[0].y ,Material[0].w };
	float materialcolor1[3] = { Material[1].x ,Material[1].y ,Material[1].w };
	float materialcolor2[3] = { Material[2].x ,Material[2].y ,Material[2].w };
	float materialcolor3[3] = { Material[3].x ,Material[3].y ,Material[3].w };


	for (int i = 0; i < 4; i++)
	{
		triangleData[i][0] = { -0.2f + (i * 0.4f),-0.8f,0.0f,1.5f };
		triangleData[i][1] = { 0.0f + (i * 0.4f),-0.4f,0.0f,1.5f };
		triangleData[i][2] = { 0.2f + (i * 0.4f),-0.8f,0.0f,1.5f };
		Material[i] = { Material[i].x ,Material[i].y ,Material[i].w , 1.0f };
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
			
			myEngine->Update();

		ImGui::Begin("material");
		ImGui::ColorEdit3("material", materialcolor);
		ImGui::ColorEdit3("material", materialcolor1);
		ImGui::ColorEdit3("material", materialcolor2);
		ImGui::ColorEdit3("material", materialcolor3);
		ImGui::End();

		Material[0].x = materialcolor[0];
		Material[0].y = materialcolor[1];
		Material[0].w = materialcolor[2];

		Material[1].x = materialcolor1[0];
		Material[1].y = materialcolor1[1];
		Material[1].w = materialcolor1[2];

		Material[2].x = materialcolor2[0];
		Material[2].y = materialcolor2[1];
		Material[2].w = materialcolor2[2];

		Material[3].x = materialcolor3[0];
		Material[3].y = materialcolor3[1];
		Material[3].w = materialcolor3[2];

			for (int i = 0; i < 4; i++) {

				triangle[i]->Update(Material[i]);

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

	CoUninitialize();

	return 0;
}