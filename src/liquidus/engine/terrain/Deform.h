#pragma once

#include "DeformResult.h"

class Deform
{
	Quad* _root;

public:
	Deform();
	~Deform();

	uint32 Create(int x, int y, int w, int h );
	DeformResult DeformByCircle( int x, int y, int radius );
	void SetUserData(int qid, INT_PTR userData);

	void ReplaceRoot(Quad* newRoot);

	template<typename Func>
	void Walk(Func& func) {
		_root->WalkQuad(func);
	}
	
private:
	void SplitByCircle(const Circle& c);
};