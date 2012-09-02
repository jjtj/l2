#pragma once

#include "Deform.h"
#include "../common/Circle.h"

using namespace std;

/**
 *	Terrain
 */
class Terrain
{
	Deform _deformQuads;
	map<int, Quad*> _leafs;

public:
	Terrain();

	void ReplaceRoot(Quad* root);

	DeformResult BreakTerrain(const Circle& c, float amplification);

	template<typename Func>
	void Query(int x, int y, int w, int h, Func& func) {
		for(auto it = _leafs.begin(); it != _leafs.end(); ++it) {
			auto q = it->second;
			if(q->HasOverwrap(x,y,w,h))
				func(q);
		}
	}
};