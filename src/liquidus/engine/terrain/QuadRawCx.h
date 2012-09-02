
#pragma once

using namespace Platform::Collections;

namespace engine {
	public ref class QuadRawCx sealed
	{
		int _x;
		int _y;
		int _w;
		int _h;
		bool _isSplit;

		Vector<QuadRawCx^>^ _children;

	public:
		QuadRawCx() {}

		void SetData(int x, int y, int w, int h, int splitMark) {
			_x = x;
			_y = y;
			_w = w;
			_h = h;
			_isSplit = (splitMark) == 1;
		}
		void AddChild(QuadRawCx^ child) {
			if(_children == nullptr)
				_children = ref new Vector<QuadRawCx^>();

			_children->Append(child);
		}

		int X() { return _x; }
		int Y() { return _y; }
		int W() { return _w; }
		int H() { return _h; }
		bool IsSplit() { return _isSplit; }
		int Count() { 
			if(_children == nullptr)
				return 0;

			return _children->Size;
		}

		QuadRawCx^ Child(int index) {
			if(_children == nullptr)
				return nullptr;

			return _children->GetAt(index);
		}
	};
};
