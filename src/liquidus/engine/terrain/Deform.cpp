#include "pch.h"
#include "Deform.h"

#include "..\common\FixedArray.h"

using namespace concurrency;
using namespace std;

Deform::Deform()
{
	_root = nullptr;
}

Deform::~Deform()
{
	if(_root != nullptr)
		delete _root;
}

uint32 Deform::Create(int x, int y, int w, int h)
{
	_ASSERT(_root == nullptr);
	_root = new Quad(x,y,w,h);
	return _root->Id();
}

void Deform::SplitByCircle(const Circle& c)
{
	_root->SplitByCircle(c);
}

DeformResult Deform::DeformByCircle(int x, 
									int y, 
									int radius)
{
	_ASSERT(this->_root != nullptr);

	Circle c((float)x,(float)y,(float)radius);
	this->SplitByCircle(c);
	
	FixedArray<uint32> delUserData(Quad::GetTotalQuads());
	FixedArray<QuadPtr> leafs(Quad::GetTotalQuads());

	auto maxBottom = 0;

	_root->WalkQuad(
		[&](Quad* q,bool isSplitted,bool isDeleted)
	{
		if(isSplitted || isDeleted)
		{
			if(isDeleted)
				maxBottom = max(q->Bottom(), maxBottom);

			if(q->HasUserData() == false)
				return;

			delUserData.Push(q->GetUserData());
			q->ClearUserData();
			return;
		}

		if(isSplitted == false
			&& isDeleted == false
			&& q->HasUserData() == false)
		{
			_ASSERT(q->IsSplitted() == false);
			_ASSERT(q->IsDeleteMarked() == false);

			leafs.Push(q);
		}
	});

	_root->DeleteAllMarked();

	// Prepare result object
	DeformResult r;
	r.maxDeformBottom = maxBottom;
	r.delUserDataCnt = delUserData.Count();
	r.delUserData = delUserData.Detach();

	r.leafsCnt = leafs.Count();
	r.leafs = leafs.Detach();

	return r;
}

//
// Traversal all the children
//
void Deform::SetUserData( int qid, INT_PTR userData )
{
	_ASSERT(_root != nullptr);
	_root->WalkQuad([=](Quad* q, bool, bool){
		if(q->Id() == qid)
			q->SetUserData(userData);
	});
}

/**
 *	Replace root
 */
void Deform::ReplaceRoot(Quad* newRoot) 
{
	if(_root != nullptr)
		delete _root;

	_root = newRoot;
}

//
// Fill mask
//
/*
void Deform::FillMask(const RawBuffer& mask, int yOffset)
{
	auto p = mask.p;
	auto w = mask.w;
	auto h = mask.h;

	_ASSERT(p != nullptr);
	_ASSERT(w>0);
	_ASSERT(h>0);

	memset(p,0,w*h*sizeof(uint8));

	_root->WalkQuad([=](Quad* q, bool isSplitted, bool isDeleteMarked)
	{
		if(isSplitted == true
			|| isDeleteMarked == true
			|| q->HasUserData() == false)
			return;

		auto x = q->X();
		auto y = q->Y();
		auto quadW = q->Width();
		auto quadH = q->Height();

		y -= yOffset;

		if(x+quadW<=0)
			return;
		if(x>=w)
			return;
		if(y+quadH<=0)
			return;
		if(y>=h)
			return;

		if(x<0)
		{
			quadW += x;
			x = 0;
		}

		if(y<0)
		{
			quadH += y;
			y=0;
		}

		if(x+quadW > w)
			quadW -= (x+quadW)-w;

		if(y+quadH > h)
			quadH -= (y+quadH) - h;

		if(quadW <= 0
			|| quadH <= 0)
			return;

		parallel_for(
			y,
			y + quadH,
			[p,w,h,x,quadW](int yIndex)
		{
			if(yIndex<0)
				return;
			if(yIndex>=h)
				return;

			uint8* pp = p + yIndex*w + x;
			memset(pp, 0xFF, quadW);
		});
	});
}

*/
