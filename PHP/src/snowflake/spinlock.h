#ifndef __SPINLOCK_H
#define __SPINLOCK_H
#include <stdint.h>

typedef volatile unsigned int atomic_t;

extern void spin_lock(atomic_t *lock, uint32_t pid);

extern void spin_unlock(atomic_t *lock, uint32_t pid);

#endif
