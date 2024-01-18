#pragma once
#include"Types.h"

// RW SpinLock
// 기본적으로 MMO에서는 SPIN LOCK 

// 왜 필요한가?
// 표준 Lock으로 사용해도 문제가 되진 않는다. 하지만 몇가지 지원하지 않는 기능을 추가하기 위해 만든다. 
// 첫번째로 기존 Lock은 재귀적으로 Lock을 잠굴 수 없다. 
// 다른 mutex버전을 사용해도 되지만 정말 가끔 상호배타적인 경우가 발생한다. 
// 그냥 공부를 위해서라도 Lock을 만들어보자 
// 
// 32비트를 사용할것, 
// [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
//

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
	void WriteUnLock();
	void ReadLock();
	void ReadUnLock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};



/* ---------------
	Lock Guard
-----------------*/


class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.ReadLock();
	}
	~ReadLockGuard() { _lock.ReadUnLock(); }


private:
	Lock& _lock;
};


class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.WriteLock();
	}
	~WriteLockGuard() { _lock.WriteUnLock(); }


private:
	Lock& _lock;
};
