#include <stdlib.h>
#include <sched.h>
#include "spinlock.h"

extern int ncpu;
extern int spin;

void spin_lock(atomic_t *lock, uint32_t pid)
{
    int i, n;

    for (;;)
    {

        if (*lock == 0 &&
            __sync_bool_compare_and_swap(lock, 0, pid))
        {
            return;
        }

        if (ncpu > 1)
        {

            for (n = 1; n < spin; n <<= 1)
            {

                for (i = 0; i < n; i++)
                {
                    __asm("pause");
                }

                if (*lock == 0 &&
                    __sync_bool_compare_and_swap(lock, 0, pid))
                {
                    return;
                }
            }
        }

        sched_yield();
    }
}

void spin_unlock(atomic_t *lock, uint32_t pid)
{
    __sync_bool_compare_and_swap(lock, pid, 0);
}
