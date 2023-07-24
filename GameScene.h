#pragma once
#include "Triangle.h"
#include "Sprite.h"

class GameScene
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Mesh mesh_;
};

