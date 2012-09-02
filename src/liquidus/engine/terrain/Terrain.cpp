#include "pch.h"
#include "Terrain.h"

#include "../common/Circle.h"

using namespace concurrency;

/**
 *	CTOR
 */
Terrain::Terrain()
{	
}

/**
 *	Break terrain
 */
DeformResult Terrain::BreakTerrain(const Circle& c,
								   float amplification)
{
	auto r = c.r * amplification;
	return _deformQuads.DeformByCircle((int)c.x, 
									   (int)c.y, 
									   (int)r);
}


void Terrain::ReplaceRoot(Quad* root) {
	_deformQuads.ReplaceRoot(root);
	_deformQuads.Walk([&](Quad* q, bool isSplitted, bool isDeleted) {
		if(q->IsValidLeaf())
			_leafs[q->Id()] = q;
	});
}
