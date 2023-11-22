#include<Windows.h>
#include "MyEngine.h"
#include "Triangle.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sphere.h"
#include "camera.h"
#include "Input.h"
#include "Audio.h"
#include "SpriteCommon.h"
#include "Sprite.h"

//Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma region 基盤システムの初期化

	// WinAppのポインタ
	WinApp* winApp = nullptr;
	// WinAppの初期化
	winApp = new WinApp();
	winApp->StartApp();

	DirectXCommon* dxCommon = nullptr;
	// DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp->GetHwnd());

	// 入力ポインタ
	Input* input = nullptr;
	// 入力初期化
	input = new Input();
	input->Initialize(winApp);

	// スプライト共通部ポインタ
	SpriteCommon* spriteCommon = nullptr;
	// スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

#pragma endregion 基盤システムの初期化

#pragma region 最初のシーンの初期化


	MyEngine* myEngine = new MyEngine();

	// ゲームシーンの初期化
	myEngine->Initialize();
	Triangle* triangle = new Triangle();
	Camera* camera = new Camera();

	uint32_t kClientWidht = 1280;
	uint32_t kClientHeight = 720;
	camera->Initialize(kClientWidht, kClientHeight);

	Vector4 Material[4] = { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 }, { 1,1,1,1 } };
	float materialcolor[3] = { Material[0].x ,Material[0].y ,Material[0].w };
	float materialcolor1[3] = { Material[1].x ,Material[1].y ,Material[1].w };
	float materialcolor2[3] = { Material[2].x ,Material[2].y ,Material[2].w };
	float materialcolor3[3] = { Material[3].x ,Material[3].y ,Material[3].w };

	for (int i = 0; i < 4; i++)
	{
		Material[i] = { Material[i].x ,Material[i].y ,Material[i].w , 1.0f };
	
	}

	Sphere sphere = { {0.0f,0.0f,0.0f},16 };

	Vector4 LeftTop = { 0.0f,0.0f,0.0f,1.0f };
	Vector4 LeftBottom = { 0.0f,360.0f,0.0f,1.0f };
	Vector4 RightTop = { 640.0f,0.0f,0.0f,1.0f };
	Vector4 RightBottom = { 640.0f,360.0f,0.0f,1.0f };

	triangle->transformationMatrixDataSphere_ = new TransformationMatrix;
	triangle->Initialize(dxCommon);
	int uvChecker = triangle->LoadTexture("resource/uvChecker.png");
	int monsterBall = triangle->LoadTexture("resource/monsterBall.png");
	bool textureChangeFlag = false;

	ModelData axisModel = triangle->LoadObjFile("resource", "axis.obj");

	// スプライトポインタ
	Sprite* sprite = new Sprite();
	// スプライト初期化
	sprite->Initialize(spriteCommon);

#pragma endregion 最初のシーンの終了

#pragma region 基盤システムの更新

	while (true) // ゲームループ
	{
		//windowのメッセージ処理
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}
		else {
			//ゲームの処理

			// 入力の更新
			input->Updata();
			
			myEngine->Update();

			ImGui::Begin("material");
			ImGui::ColorEdit3("material", materialcolor);
			ImGui::ColorEdit3("material", materialcolor1);
			ImGui::ColorEdit3("material", materialcolor2);
			ImGui::ColorEdit3("material", materialcolor3);
			ImGui::End();

			ImGui::Begin("SphereTexture");
			ImGui::Checkbox("texture", &textureChangeFlag);
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

			if (textureChangeFlag == true) {
				uvChecker = monsterBall;
			}
			else {
				monsterBall = uvChecker;
			}

			//triangle->TriangleImGui();

			camera->Updata();

			/*triangle->DrawSprite(LeftTop, LeftBottom, RightTop, RightBottom, uvChecker, Material[0]);*/
			triangle->DrawModel(camera->transformMatrix_, Material[0]);
			//triangle->DrawModel(axisModel, { 0,0,0 }, camera->transformMatrix_, Material[0]);

			triangle->ResetVertex();
			myEngine->UpdateEnd();
		}
	}
#pragma endregion 基盤システムの更新

#pragma region 基盤システムの終了
	// 入力開放
	delete input;

	triangle->Finalize();
	delete spriteCommon;
	delete sprite;
	winApp->EndApp();
	myEngine->Finalize();
	dxCommon->PostDraw();

	CoUninitialize();
#pragma endregion 基盤システムの終了

	return 0;
}