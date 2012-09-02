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

	void ReplaceRoot(Quad* root) {
		_deformQuads.ReplaceRoot(root);
	}

	DeformResult BreakTerrain(const Circle& c, float amplification);

	template<typename Func>
	void Query(int x, int y, int w, int h, Func& func) {
		_deformQuads.Walk([&](Quad* q, bool isSplitted, bool isDeleted) {
			if(q->IsValidLeaf()) {
				if(q->HasOverwrap(x,y,w,h)) {
					func(q);
				}
			}
		});
	}
};