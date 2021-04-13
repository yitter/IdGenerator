#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "snowflake/snowflake.h"

#if defined(WIN32)
#include "windows.h"
#endif

#define THREAD 2
#define TOTAL 50000

static snowflake snowf = {1, 0, 1, 6, 12, 0, 5, 2000};
static snowflake *flake = &snowf;

uint64_t arr[TOTAL];

static uint64_t index = 0;

uint64_t compar(const void *a, const void *b)
{
  return (*(uint64_t *)a - *(uint64_t *)b);
}

uint64_t containsDuplicate()
{
  uint32_t i, j;
  qsort(arr, TOTAL, sizeof(uint64_t), compar);
  for (i = 0, j = 1; j < TOTAL; i++, j++)
  {
    if (arr[i] > 0 && arr[j] > 0 && arr[i] == arr[j])
    {
      return j;
    }
  }
  return 0;
}

void run()
{
  for (int i = 0; i < TOTAL / THREAD; i++)
  {
    arr[__sync_fetch_and_add(&index, 1)] = NextId(flake);
  }
}

int main()
{
  flake->Method = 1;
  Config(flake);
  pthread_t tid[THREAD];
  struct timeval t_start, t_end;
  while (1)
  {
    // clock_gettime(CLOCK_REALTIME, &t_start);

    // for (int i = 0; i < THREAD; i++)
    // {
    //   if (pthread_create(&tid[i], NULL, (void *)run, NULL) != 0)
    //   {
    //     printf("thread creation failed\n");
    //     exit(1);
    //   }
    // }

    // for (int i = 0; i < THREAD; i++)
    // {
    //   pthread_join(tid[i], NULL); //等待线程结束
    // }

    // clock_gettime(CLOCK_REALTIME, &t_end);

    // printf("%d 线程 %s，总共：%d，%lf ms\n", THREAD, flake->Method == 1 ? "漂移" : "传统", index, (double)(t_end.tv_nsec - t_start.tv_nsec) / 1000000.0);

    // uint64_t re = containsDuplicate();
    // if (re > 0)
    // {
    //   printf("有重复数据:%ld,%ld\n", arr[re - 1], arr[re]);
    // }
    // else
    // {
    //   printf("没有重复数据\n");
    // }

    // for (int i = 0; i < TOTAL; i++)
    // {
    //   arr[i] = 0;
    // }
    // index = 0;

    gettimeofday(&t_start, NULL);
    for (int i = 0; i < TOTAL; i++)
    {
      NextId(flake);
    }
    gettimeofday(&t_end, NULL);
    printf("单线程 %s，总共：%d，%lf ms\n", flake->Method == 1 ? "漂移" : "传统", TOTAL, (double)(t_end.tv_usec - t_start.tv_usec) / 1000.0);
    sleep(1);
  }

  return 1;
}