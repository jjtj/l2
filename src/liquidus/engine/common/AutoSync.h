#pragma once

#include "Sync.h"

struct AutoSync
{
	Sync& _sync;
	AutoSync(Sync& s) : _sync(s) {
		_sync.Lock();
	}
	~AutoSync() {
		_sync.Unlock();
	}
};

#define AUTOSYNC	\
	AutoSync _autoSyncMacro(*_sync)