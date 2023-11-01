#pragma once
#include <vector>
#include "VertexData.h"

struct ModelData {
	std::vector<VertexData> vertices;
	int textureIndex;
	MaterialData material;
};
