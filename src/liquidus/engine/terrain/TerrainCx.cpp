#include "pch.h"
#include "TerrainCx.h"
#include "../Common/SyncCriticalSection.h"
#include "../Common/AutoSync.h"

using namespace engine;


TerrainCx::TerrainCx()
{	
}

void TerrainCx::Build(QuadRawCx^ root)
{
	auto q = BuildRecursive(root);
	_terrain.ReplaceRoot(q);
}

Quad* TerrainCx::BuildRecursive(QuadRawCx^ q)
{
	Quad* quad = new Quad(q->X(), q->Y(), q->W(), q->H());

	int cnt = q->Count();
	for(int i=0;i<cnt;++i) {

		auto qq = q->Child(i);
		auto cc = BuildRecursive(qq);
		quad->AddChild(cc);
	}

	return quad;
}

IVector<int>^ TerrainCx::Query(int x, int y, int w, int h)
{
	auto v = ref new Vector<int>();
	_terrain.Query(x,y,w,h, [&](Quad* q) {
		_ASSERTE(q->IsValidLeaf());

		v->Append(q->X());
		v->Append(q->Y());
		v->Append(q->Width());
		v->Append(q->Height());
	});

	return v;
}

