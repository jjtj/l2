#pragma once

#include "Quad.h"

struct DeformResult
{
	int maxDeformBottom;

	uint32* delUserData;
	int delUserDataCnt;

	QuadPtr* leafs;
	int leafsCnt;

	DeformResult() {
		delUserData = nullptr;
		leafs = nullptr;
	}

	void Destroy() {
		if(delUserData)
			delete [] delUserData;

		if(leafs)
			delete [] leafs;
	}
};