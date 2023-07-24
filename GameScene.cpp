#include "GameScene.h"

void GameScene::Initialize()
{
    DirectXCommon* dxClass = new DirectXCommon(); // 仮想的な DirectXCommon クラスのインスタンス生成

    // Meshクラスの初期化を行う（dxClassは仮想的な DirectXCommon クラスのポインタ）
    mesh_.Initialize(dxClass);
}

void GameScene::Update()
{
    // ゲームの更新処理a
    // mesh_.Update()を呼び出す必要がある場合は呼び出す
}

void GameScene::Draw()
{
    // mesh_.Draw()を呼び出す
    Vector4 a(0, 0, 0);
    Vector4 b(1, 0, 0);
    Vector4 c(0, 1, 0);
    mesh_.Draw(a, b, c);
}