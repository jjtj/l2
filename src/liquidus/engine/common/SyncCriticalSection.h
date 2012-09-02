#pragma once

#include "Sync.h"

struct SyncCriticalSection : public Sync
{
	CRITICAL_SECTION _cs;
	SyncCriticalSection()
	{
		::InitializeCriticalSectionEx(&_cs, 100, 0);
	}
	~SyncCriticalSection()
	{
		::DeleteCriticalSection(&_cs);
	}

	virtual void Lock()
	{
		::EnterCriticalSection(&_cs);
	}

	virtual void Unlock()
	{
		::LeaveCriticalSection(&_cs);
	}
};