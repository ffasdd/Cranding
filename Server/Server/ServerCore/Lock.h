#pragma once
#include"Types.h"

// RW SpinLock
// 기본적으로 MMO에서는 SPIN LOCK 


// W -> R (0)
// R -> W (X)
class Lock
{
	enum : uint32
	{
		ACQURE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000,
	};
public: 
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnLock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};

