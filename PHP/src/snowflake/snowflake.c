#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include "snowflake.h"
#include "spinlock.h"

#if defined(WIN32)
#include "windows.h"
#endif

static void EndOverCostAction(uint64_t useTimeTick, snowflake *flake);
static uint64_t NextOverCostId(snowflake *flake);
static uint64_t NextNormalId(snowflake *flake);
static uint64_t GetCurrentTimeTick(snowflake *flake);
static uint64_t GetNextTimeTick(snowflake *flake);
static uint64_t CalcId(snowflake *flake);
static uint64_t CalcTurnBackId(snowflake *flake);

int ncpu;
uint16_t spin = 2048;
uint32_t pid = 0;

void Config(snowflake *flake)
{
  if (pid == 0)
  {
    pid = (uint32_t)getpid();
#if defined(WIN32)
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    ncpu = sysInfo.dwNumberOfProcessors;
#else
    ncpu = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    if (ncpu <= 0)
    {
      ncpu = 1;
    }
  }
  flake->WorkerIdBitLength = flake->WorkerIdBitLength == 0 ? 6 : flake->WorkerIdBitLength;
  flake->SeqBitLength = flake->SeqBitLength == 0 ? 6 : flake->SeqBitLength;
  flake->MaxSeqNumber = flake->MaxSeqNumber > 0 ? flake->MaxSeqNumber : (-1L << flake->SeqBitLength) ^ -1L;
  flake->BaseTime = flake->BaseTime != 0 ? flake->BaseTime : 1577808000000;
  flake->_TimestampShift = (uint8_t)(flake->WorkerIdBitLength + flake->SeqBitLength);
  flake->_CurrentSeqNumber = flake->MinSeqNumber;
  if (flake->MaxSeqNumber <= flake->MinSeqNumber)
  {
    flake->MinSeqNumber = 0;
  }
}

void inline EndOverCostAction(uint64_t useTimeTick, snowflake *flake)
{
  if (flake->_TermIndex > 10000)
  {
    flake->_TermIndex = 0;
  }
}

uint64_t inline NextOverCostId(snowflake *flake)
{
  uint64_t currentTimeTick = GetCurrentTimeTick(flake);
  if (currentTimeTick > flake->_LastTimeTick)
  {
    EndOverCostAction(currentTimeTick, flake);
    flake->_LastTimeTick = currentTimeTick;
    flake->_CurrentSeqNumber = flake->MinSeqNumber;
    flake->_IsOverCost = 0;
    flake->_OverCostCountInOneTerm = 0;
    flake->_GenCountInOneTerm = 0;
    return CalcId(flake);
  }
  if (flake->_OverCostCountInOneTerm > flake->TopOverCostCount)
  {
    EndOverCostAction(currentTimeTick, flake);
    flake->_LastTimeTick = GetNextTimeTick(flake);
    flake->_CurrentSeqNumber = flake->MinSeqNumber;
    flake->_IsOverCost = 0;
    flake->_OverCostCountInOneTerm = 0;
    flake->_GenCountInOneTerm = 0;
    return CalcId(flake);
  }
  if (flake->_CurrentSeqNumber > flake->MaxSeqNumber)
  {
    flake->_LastTimeTick++;
    flake->_CurrentSeqNumber = flake->MinSeqNumber;
    flake->_IsOverCost = 1;
    flake->_OverCostCountInOneTerm++;
    flake->_GenCountInOneTerm++;
    return CalcId(flake);
  }

  flake->_GenCountInOneTerm++;
  return CalcId(flake);
}

uint64_t inline NextNormalId(snowflake *flake)
{
  uint64_t currentTimeTick = GetCurrentTimeTick(flake);
  if (currentTimeTick < flake->_LastTimeTick)
  {
    if (flake->_TurnBackTimeTick < 1)
    {
      flake->_TurnBackTimeTick = flake->_LastTimeTick - 1;
      flake->_TurnBackIndex++;
      if (flake->_TurnBackIndex > 4)
      {
        flake->_TurnBackIndex = 1;
      }
    }
    return CalcTurnBackId(flake);
  }
  if (flake->_TurnBackTimeTick > 0)
  {
    flake->_TurnBackTimeTick = 0;
  }
  if (currentTimeTick > flake->_LastTimeTick)
  {
    flake->_LastTimeTick = currentTimeTick;
    flake->_CurrentSeqNumber = flake->MinSeqNumber;
    return CalcId(flake);
  }
  if (flake->_CurrentSeqNumber > flake->MaxSeqNumber)
  {
    flake->_TermIndex++;
    flake->_LastTimeTick++;
    flake->_CurrentSeqNumber = flake->MinSeqNumber;
    flake->_IsOverCost = 1;
    flake->_OverCostCountInOneTerm = 1;
    flake->_GenCountInOneTerm = 1;
    return CalcId(flake);
  }
  return CalcId(flake);
}

uint64_t inline GetCurrentTimeTick(snowflake *flake)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (uint64_t)((t.tv_sec * 1000 + t.tv_usec / 1000) - flake->BaseTime);
}

uint64_t inline GetNextTimeTick(snowflake *flake)
{
  uint64_t tempTimeTicker = GetCurrentTimeTick(flake);
  while (tempTimeTicker <= flake->_LastTimeTick)
  {
    tempTimeTicker = GetCurrentTimeTick(flake);
  }
  return tempTimeTicker;
}

uint64_t inline CalcId(snowflake *flake)
{
  uint64_t result = (flake->_LastTimeTick << flake->_TimestampShift) + (flake->WorkerId << flake->SeqBitLength) + (flake->_CurrentSeqNumber);
  flake->_CurrentSeqNumber++;
  return result;
}

uint64_t inline CalcTurnBackId(snowflake *flake)
{
  uint64_t result = (flake->_LastTimeTick << flake->_TimestampShift) + (flake->WorkerId << flake->SeqBitLength) + (flake->_TurnBackTimeTick);
  flake->_TurnBackTimeTick--;
  return result;
}

uint64_t inline NextSonwId(snowflake *flake)
{
  uint64_t currentTimeTick = GetCurrentTimeTick(flake);
  if (flake->_LastTimeTick == currentTimeTick)
  {
    flake->_CurrentSeqNumber++;
    if (flake->_CurrentSeqNumber > flake->MaxSeqNumber)
    {
      flake->_CurrentSeqNumber = flake->MinSeqNumber;
      currentTimeTick = GetNextTimeTick(flake);
    }
  }
  else
  {
    flake->_CurrentSeqNumber = flake->MinSeqNumber;
  }
  flake->_LastTimeTick = currentTimeTick;
  return (uint64_t)((currentTimeTick << flake->_TimestampShift) | (flake->WorkerId << flake->SeqBitLength) | flake->_CurrentSeqNumber);
}

uint64_t inline GetId(snowflake *flake)
{
  return flake->Method == 1 ? (flake->_IsOverCost != 0 ? NextOverCostId(flake) : NextNormalId(flake)) : NextSonwId(flake);
}

uint64_t NextId(snowflake *flake)
{
  spin_lock(&flake->_Lock, pid);
  uint64_t id = GetId(flake);
  spin_unlock(&flake->_Lock, pid);
  return id;
}

uint64_t *NextNumId(snowflake *flake, uint32_t num)
{
  uint64_t *arr = (uint64_t *)malloc(sizeof(uint64_t) * num);
  spin_lock(&flake->_Lock, pid);
  for (uint32_t i = 0; i < num; i++)
  {
    arr[i] = GetId(flake);
  }
  spin_unlock(&flake->_Lock, pid);
  return arr;
}
