#pragma once
#define OUT
// 억지로 크래쉬를 내고싶은 상황에서 작동하게 끔 
#define CRASH(cause)								\
{													\
	uint32* crash = nullptr;						\
	__analysis_assume(crash != nullptr);			\
	*crash = 0xDEADBEEF;							\
}													\

#define ASSERT_CRASH(expr)							\
{													\
	if (!(expr))									\
	{												\
		CRASH("ASSERT_CRASH");						\
		__analysis_assume(expr);					\
	}												\
}													\