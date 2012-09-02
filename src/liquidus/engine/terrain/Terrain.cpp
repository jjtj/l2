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

