#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "shm.h"

#ifdef MAP_ANON

int shm_alloc(struct shm *shm)
{
    shm->addr = (void *)mmap(NULL, shm->size,
                             PROT_READ | PROT_WRITE,
                             MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (shm->addr == NULL)
    {
        return -1;
    }

    return 0;
}

void shm_free(struct shm *shm)
{
    if (shm->addr)
    {
        munmap((void *)shm->addr, shm->size);
    }
}

#else

int shm_alloc(struct shm *shm)
{
    int fd;

    fd = open("/dev/zero", O_RDWR);
    if (fd == -1)
    {
        return -1;
    }

    shm->addr = (void *)mmap(NULL, shm->size,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0);

    close(fd);

    if (shm->addr == NULL)
    {
        return -1;
    }

    return 0;
}

void shm_free(struct shm *shm)
{
    if (shm->addr)
    {
        munmap((void *)shm->addr, shm->size);
    }
}

#endif
