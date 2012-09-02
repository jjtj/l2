#pragma once

struct Sync
{
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};