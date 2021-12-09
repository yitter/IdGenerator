#include <stdlib.h>
#ifdef WIN32
#include "windows.h"
#else
#include <sched.h>
#endif

#include "spinlock.h"

extern int ncpu;
extern int spin;

void spin_lock(atomic_t* lock, uint32_t pid)
{
	int i, n;

	for (;;)
	{

		if (*lock == 0 &&
#ifdef WIN32
			InterlockedCompareExchange(lock, pid, 0) == 0
#else
			__sync_bool_compare_and_swap(lock, 0, pid)
#endif 
			)
		{
			return;
		}

		if (ncpu > 1)
		{

			for (n = 1; n < spin; n <<= 1)
			{

				for (i = 0; i < n; i++)
				{
#ifdef WIN32
					MemoryBarrier();
#else 
					__asm("pause");
#endif 
				}

				if (*lock == 0 &&
#ifdef WIN32
					InterlockedCompareExchange(lock, pid, 0) == 0
#else
					__sync_bool_compare_and_swap(lock, 0, pid)
#endif 
					)
				{
					return;
				}
			}
		}
#ifdef WIN32
		SwitchToThread();
#else
		sched_yield();
#endif
	}
}

void spin_unlock(atomic_t* lock, uint32_t pid)
{
#ifdef WIN32
	InterlockedCompareExchange(lock, 0, pid);
#else
	__sync_bool_compare_and_swap(lock, pid, 0);
#endif 
}
