#pragma once

#include "Terrain.h"
#include "QuadRawCx.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

namespace engine {
	public ref class TerrainCx sealed
	{
		Terrain _terrain;

	public:
		TerrainCx();

		void Build(engine::QuadRawCx^ root);
		IVector<int>^ Query(int x, int y, int w, int h);

	private:
		Quad* BuildRecursive(QuadRawCx^ q);
	};
};
