#ifndef __SPINLOCK_H
#define __SPINLOCK_H
#include <stdint.h>

typedef volatile unsigned int atomic_t;

extern void spin_lock(atomic_t *lock, uint32_t pid);

extern void spin_unlock(atomic_t *lock, uint32_t pid);

#if defined(WIN32)
#define atomic_cpu_pause() MemoryBarrier();
#elif defined(__x86_64__)
#define atomic_cpu_pause() __asm__ __volatile__("pause")
#elif defined(__aarch64__)
#define atomic_cpu_pause() __asm__ __volatile__("yield")
#else
#define atomic_cpu_pause()
#endif

#endif
