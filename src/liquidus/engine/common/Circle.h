#pragma once

#include "Point.h"

struct Circle
{
	float x;
	float y;
	float r;
	float rr;

	Circle(float xx,float yy, float radius) {
		x = xx;
		y = yy;
		r = radius;
		rr = radius*radius;
	}

	bool IsIn(const Point& pt) const {
		return IsIn((float)pt.x, (float)pt.y);
	}

	bool IsIn(float xx, float yy) const
	{
		auto dist = (xx-x)*(xx-x) + (yy-y)*(yy-y);
		return dist<=rr;
	}

	bool hasIntersection(const Point& p1, const Point& p2) const {
		return hasIntersection((float)p1.x,(float)p1.y,(float)p2.x,(float)p2.y);
	}
	bool hasIntersection(float x1, float y1, float x2, float y2) const
	{
		float a = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
		float b = 2 * ((x2 - x1) * (x1 - this->x) + (y2 - y1) * (y1 - this->y));
		float c =
			(this->x * this->x + this->y * this->y) + x1 * x1 + y1 * y1
			- 2 * (this->x * x1 + this->y * y1) - this->rr;

		float det = b * b - 4 * a * c;
		if (det < 0)
			return false;

		if (det == 0)
		{
			float u = -b / (2 * a);
			return (u >= 0.0) && (u <= 1.0);
		}

		det = std::sqrt(det);
		float u1 = (-b + det) / (2 * a);
		if (u1 >= 0.0 && u1 <= 1.0)
			return true;

		float u2 = (-b - det) / (2 * a);
		if (u2 >= 0.0 && u2 <= 1.0)
			return true;

		return false;
	}
};