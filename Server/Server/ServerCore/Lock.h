#pragma once
#include"Types.h"

// RW SpinLock
// �⺻������ MMO������ SPIN LOCK 

// �� �ʿ��Ѱ�?
// ǥ�� Lock���� ����ص� ������ ���� �ʴ´�. ������ ��� �������� �ʴ� ����� �߰��ϱ� ���� �����. 
// ù��°�� ���� Lock�� ��������� Lock�� �ᱼ �� ����. 
// �ٸ� mutex������ ����ص� ������ ���� ���� ��ȣ��Ÿ���� ��찡 �߻��Ѵ�. 
// �׳� ���θ� ���ؼ��� Lock�� ������ 
// 
// 32��Ʈ�� ����Ұ�, 
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
