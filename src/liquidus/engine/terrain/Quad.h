#pragma once

#include "..\Common\Point.h"
#include "..\Common\Circle.h"

const float VERY_SMALL_QUAD_SIZE = 1;

class Quad
{
	int _id;
	int _x;
	int _y;
	int _w;
	int _h;

	std::vector<Quad*>* _quads;
	bool _deleteMark;
	INT_PTR _userData;
	int _deleteCount;

public:
	Quad(void);
	Quad(int x,int y, int w,int h);
	~Quad(void);

	static long GetTotalQuads();

	int Id() const { return _id; }
	int X() const { return _x; }
	int Y() const { return _y;}
	int Width() const { return _w;}
	int Height() const { return _h; }
	int CenterX() const { return this->_x + this->_w/2;}
	int CenterY() const { return this->_y + this->_h/2;}
	int Bottom() const { return this->_y + this->_h; }
	bool IsSplitted() const {
		return
			_quads != nullptr
			&& _quads->empty() == false;
	}
	bool IsTooSmall() const {
		return this->_w <= VERY_SMALL_QUAD_SIZE
			&& this->_h <= VERY_SMALL_QUAD_SIZE;
	}

	void AddChild(Quad* q) {
		if(this->_quads == nullptr)
			this->_quads = new std::vector<Quad*>();

		this->_quads->push_back(q);
	}

	void MarkDelete();
	bool IsDeleteMarked() const {
		return this->_deleteMark;
	}
	bool IsValidLeaf() const {
		return IsSplitted() == false 
			&& IsDeleteMarked() == false;
	}

	void SplitByCircle(const Circle& c);
	void ToPoints(Point* points);

	template<typename CallbackFunc>
	void WalkQuad(CallbackFunc& func)
	{
		func(this, this->IsSplitted(), this->IsDeleteMarked());
		if(IsSplitted())
		{
			concurrency::parallel_for_each(_quads->begin(), _quads->end(),
				[=](Quad* q) {
					q->WalkQuad(func);
			});
		}
	}

	void DeleteAllMarked();
	void SetUserData(INT_PTR udata);
	INT_PTR GetUserData() const;
	void ClearUserData();
	bool HasUserData() const;

	bool HasOverwrap(int x2, int y2, int w2, int h2) const {
		if(w2 > this->_w && h2 > this->_h) {
			// this Object is smaller one.
			bool nooverwrap = (this->_x + this->_w) <x2 
				|| this->_x >= (x2+w2) 
				|| (this->_y + this->_h) < y2
				|| this->_y >= (y2+h2);
			return nooverwrap == false;
		}
		else {
			bool nooverwrap = (x2+w2)<=this->_x
				|| x2 >= (this->_x + this->_w)
				|| (y2+h2) <= this->_y
				|| y2 >= (this->_y + this->_h);
			return nooverwrap == false;
		}
	}

private:
	Point LT() const { return Point(this->_x,this->_y); }
	Point RT() const { return Point(this->_x+this->_w,this->_y); }
	Point RB() const { return Point(this->_x+this->_w,this->_y+this->_h); }
	Point LB() const { return Point(this->_x,this->_y+this->_h); }
	void Split();
	void CheckPointsMembership(const Circle& c, bool* checkPoints);
	bool IsIn(int x, int y) const
	{
		return (x>=_x && x<=(_x+_w))
			&& (y>=_y && y<=(_y+_h));
	}
};

typedef Quad*	QuadPtr;

inline void Quad::ToPoints( Point* points )
{
	_ASSERT(points != nullptr);
	points[0] = LT();
	points[1] = RT();
	points[2] = RB();
	points[3] = LB();
}

inline void Quad::SetUserData( INT_PTR udata )
{
	//
	// Parallel processing can cause duplicated set
	//
	_ASSERT(this->_userData == INT_PTR_MAX
		|| this->_userData == udata);
	this->_userData = udata;
}

inline INT_PTR Quad::GetUserData() const
{
	return this->_userData;
}

inline void Quad::ClearUserData()
{
	this->_userData = INT_PTR_MAX;
}

inline bool Quad::HasUserData() const
{
	return this->_userData != INT_PTR_MAX;
}