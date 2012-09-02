#include "pch.h"
#include "Quad.h"

using namespace std;
using namespace concurrency;

volatile long gTotalCnt = 0;
volatile long gAccumulatedCnt = 0;
long Quad::GetTotalQuads()
{
    return gTotalCnt;
}

Quad::Quad(void)
{
    InterlockedIncrement(&gTotalCnt);

    this->_id = InterlockedIncrement(&gAccumulatedCnt);
    this->_deleteMark = false;
    this->_userData = INT_PTR_MAX;
    this->_quads = nullptr;
    this->_deleteCount = 0;
}

Quad::Quad(int x,int y, int w,int h )
{
    this->_id = InterlockedIncrement(&gAccumulatedCnt);
    this->_x = x;
    this->_y = y;
    this->_w = w;
    this->_h = h;
    this->_deleteMark = false;
    this->_userData = INT_PTR_MAX;
    this->_quads = nullptr;
    this->_deleteCount = 0;

    InterlockedIncrement(&gTotalCnt);
}

Quad::~Quad(void)
{
    InterlockedDecrement(&gTotalCnt);
    if(this->IsSplitted())
    {
        for(auto it = _quads->begin();it != _quads->end(); ++it)
        {
            delete (*it);
        }

        delete _quads;
    }
}

void Quad::Split()
{
    _ASSERT(this->_quads == nullptr);
    this->_quads = new std::vector<Quad*>();

    int wh = this->_w/2;
    int hh = this->_h/2;

    this->_quads->push_back(new Quad(_x,_y,wh,hh));
    this->_quads->push_back(new Quad(_x+wh,_y,this->_w - wh,hh));
    this->_quads->push_back(new Quad(_x+wh,_y+hh,this->_w - wh,this->_h - hh));
    this->_quads->push_back(new Quad(_x,_y+hh,wh,this->_h - hh));
}

void Quad::MarkDelete()
{
    _deleteMark = true;
    if(this->IsSplitted())
    {	
#if defined(NO_PARALLEL)
		int cnt = _quads->size();
		for(int i=0;i<cnt;++i) {
			auto q = _quads->at(i);
			q->MarkDelete();
		}
#else
        parallel_for(0,(int)_quads->size(),
            [=](int index)
        {
            Quad* q = _quads->operator[](index);
            q->MarkDelete();
        });
#endif
    }
}

void Quad::CheckPointsMembership( const Circle& c, bool* checkPoints )
{
    _ASSERT(checkPoints != nullptr);

    checkPoints[0] = c.IsIn(this->LT());
    checkPoints[1] = c.IsIn(this->RT());
    checkPoints[2] = c.IsIn(this->RB());
    checkPoints[3] = c.IsIn(this->LB());
}

void Quad::SplitByCircle( const Circle& c )
{
    bool checkPoints[4] = {false};
    CheckPointsMembership(c, checkPoints);
    bool isOutside =
        (checkPoints[0] == false)
        && (checkPoints[1] == false)
        && (checkPoints[2] == false)
        && (checkPoints[3] == false);
    if(isOutside) {
        // circle can be fully inside of quad.
        if(IsIn((int)c.x,(int)c.y) == false) {
            // yeah.
            // circle is surely outside of rectangle.
            // But still circle can be partially overwrapped
            // with rectangle.
            //
            // Let's check one more time whether circle overwrapped or not.
            bool inter = c.hasIntersection(LT(),RT())
                || c.hasIntersection(RT(),RB())
                || c.hasIntersection(RB(),LB())
                || c.hasIntersection(LB(), LT());
            if(false == inter)
                return;
        }
    }
    else {
        // Is quad fully inside of circle?
        bool isInside = checkPoints[0]
        && checkPoints[1]
        && checkPoints[2]
        && checkPoints[3];
        if(isInside)
        {
            this->MarkDelete();
            return;
        }
    }

    // The quad is partially covered by circle.
    // But if the quad is too small to split more,
    // let's assume this quad is covered by circle.
    if(this->IsTooSmall())
    {
        this->MarkDelete();
        return;
    }

    // Split if it was not splited
    if(this->IsSplitted() == false)
        this->Split();

    bool allDeleted = true;
    _ASSERT(_quads->empty() == false);

#if defined(NO_PARALLEL)
	int cnt = _quads->size();
	for(int i=0;i<cnt;++i) {
		auto q = _quads->at(i);
		q->SplitByCircle(c);
		allDeleted = allDeleted && q->IsDeleteMarked();
	}
#else
    parallel_for(
        0,
        (int)_quads->size(),
        [=, &allDeleted](int index)
    {
        auto q = (*_quads)[index];
        q->SplitByCircle(c);
        allDeleted = allDeleted && q->IsDeleteMarked();
    });
#endif

    //
    // All children is deleted.
    // So current ndoe should be deleted also
    //
    if(allDeleted)
        this->_deleteMark = true;
}

void Quad::DeleteAllMarked()
{
    _ASSERT(this->IsDeleteMarked() == false);
    if(IsSplitted() == false)
        return;

    const int MAX_QUADS = 12;
    Quad* pool[MAX_QUADS];
    volatile LONG poolCnt = -1;

#if defined(NO_PARALLEL)
	int cnt = _quads->size();
	for(int index=0;index<cnt;++index) {
#else
    parallel_for(0,
        (int)_quads->size(),
        [=,&poolCnt, &pool](int index)
    {
#endif
		auto q = _quads->at(index);
        if(q->IsDeleteMarked() == false)
        {
            int index = InterlockedIncrement(&poolCnt);
            pool[index] = q;

            q->DeleteAllMarked();
        }
        else
        {
            delete q;
        }
    
#if defined(NO_PARALLEL)
	}
#else
		});
#endif

    // pool count started with 0.
    // so let's increase one.
    poolCnt++;

    _quads->clear();
    for(int i=0;i<poolCnt;++i)
    {
        auto p = pool[i];
        _ASSERT(p != nullptr);
        _ASSERT(p->IsDeleteMarked() == false);
        _quads->push_back(pool[i]);
    }

    _ASSERT(_quads->empty() == false);
}