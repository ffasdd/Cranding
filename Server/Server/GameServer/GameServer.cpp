#include"pch.h"

#include <iostream>
#include"CorePch.h"
#include"CoreMacro.h"
#include"ThreadManager.h"
#include"RefCounting.h"

// 스마트 포인터의 필요성을 언제 느낄까? 

using KnightRef = TSharedPtr<class Knight>;

class Knight : public RefCountable
{
public:
	Knight()
	{
		cout << " Knight () " << endl;

	}
	~Knight()
	{
		cout << " ~Knight ()" << endl;
	}

	void SetTarget(KnightRef target)
	{
		_target = target;
	}
	KnightRef _target = nullptr;
};

int main()
{
	// 1) 이미 만들어진 클래스 대상으로 사용 불가하다. RefCountable ( 제작 ) 
	// 2) 순환 문제 (Cycle)
	//
	KnightRef k1(new Knight());
	k1->ReleaseRef();


}

