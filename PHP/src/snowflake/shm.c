#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#ifdef WIN32
#include "windows.h"
#else
#include <sys/mman.h>
#include <strings.h>
#endif
#include "shm.h"

#ifdef WIN32
#define NAME "SnowDrift"
static HANDLE hMapFile;

int shm_alloc(struct shm* shm) {
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		shm->size,
		NAME
	);

	if (hMapFile == NULL)
	{
		return 0;
	}

	LPVOID pBuffer = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, shm->size);

	if (pBuffer == NULL)
	{
		CloseHandle(hMapFile);
		return 0;
	}
	memset((char*)pBuffer, 0, shm->size);
	shm->addr = (void*)pBuffer;
	return 1;
}

void shm_free(struct shm* shm) {
	UnmapViewOfFile(shm->addr);
	CloseHandle(hMapFile);
}

#elif defined(MAP_ANON)

int shm_alloc(struct shm* shm)
{
	shm->addr = (void*)mmap(NULL, shm->size,
		PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS | MAP_SHARED, -1, 0);

	if (shm->addr == NULL)
	{
		return -1;
	}
	bzero(shm->addr, shm->size);
	return 0;
}

void shm_free(struct shm* shm)
{
	if (shm->addr)
	{
		munmap((void*)shm->addr, shm->size);
	}
}

#else

int shm_alloc(struct shm* shm)
{
	int fd;

	fd = open("/dev/zero", O_RDWR);
	if (fd == -1)
	{
		return -1;
	}

	shm->addr = (void*)mmap(NULL, shm->size,
		PROT_READ | PROT_WRITE,
		MAP_SHARED, fd, 0);

	close(fd);

	if (shm->addr == NULL)
	{
		return -1;
	}
	bzero(shm->addr, shm->size);
	return 0;
}

void shm_free(struct shm* shm)
{
	if (shm->addr)
	{
		munmap((void*)shm->addr, shm->size);
	}
}

#endif
