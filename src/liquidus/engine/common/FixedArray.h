#pragma once

template<class T>
struct FixedArray {

	T*	_p;
	long _cnt;
	long _max;

	FixedArray(long maxLen) {
		_max = maxLen;
		_p = new T[maxLen];
		_cnt = 0;
	}
	~FixedArray() {
		if(_p)
			delete [] _p;
	}

	void Push(const T& v) {
		long oldCnt = ::InterlockedIncrement(&_cnt);
		_p[oldCnt] = v;
	}

	int Count() const { return _cnt; }

	T* ToArray() { return p; }

	void Reset() {
		_cnt = 0;
	}
	T* Detach() {
		if(_p) {
			auto pp = _p;
			_p = nullptr;
			return pp;
		}

		return nullptr;
	}
	
};
