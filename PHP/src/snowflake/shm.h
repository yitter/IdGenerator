#ifndef __SHM_H
#define __SHM_H

struct shm
{
    void *addr;
    size_t size;
};

int shm_alloc(struct shm *shm);

void shm_free(struct shm *shm);

#endif
