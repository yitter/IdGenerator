#ifdef WIN32
#include "windows.h"
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "snowflake.h"
#include "spinlock.h"

// static void EndOverCostAction(uint64_t useTimeTick, snowflake *flake);
static inline uint64_t NextOverCostId(snowflake *flake);
static inline uint64_t NextNormalId(snowflake *flake);
static inline uint64_t GetCurrentTimeTick(snowflake *flake);
static inline uint64_t GetNextTimeTick(snowflake *flake);
static inline uint64_t CalcId(snowflake *flake);
static inline uint64_t CalcTurnBackId(snowflake *flake);
static inline uint64_t GetSysCurrentTime();

int ncpu;
uint16_t spin = 2048;
uint32_t pid = 0;

void Config(snowflake *flake)
{
	if (pid == 0)
	{
#ifdef WIN32
		pid = (uint32_t)GetCurrentProcessId();
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		ncpu = sysInfo.dwNumberOfProcessors;
#else
		pid = (uint32_t)getpid();
		ncpu = sysconf(_SC_NPROCESSORS_ONLN);
#endif
		if (ncpu <= 0)
		{
			ncpu = 1;
		}
	}
	if (flake->BaseTime == 0)
	{
		flake->BaseTime = 1582136402000;
	}
	else if (flake->BaseTime < 631123200000 || flake->BaseTime > GetSysCurrentTime())
	{
		perror("BaseTime error.");
		exit(1);
	}

	// 2.WorkerIdBitLength
	if (flake->WorkerIdBitLength <= 0)
	{
		perror("WorkerIdBitLength error.(range:[1, 21])");
		exit(1);
	}
	if (flake->SeqBitLength + flake->WorkerIdBitLength > 22)
	{
		perror("error：WorkerIdBitLength + SeqBitLength <= 22");
		exit(1);
	}
	else
	{
		flake->WorkerIdBitLength = flake->WorkerIdBitLength <= 0 ? 6 : flake->WorkerIdBitLength;
	}

	// 3.WorkerId
	uint32_t maxWorkerIdNumber = (1 << flake->WorkerIdBitLength) - 1;
	if (maxWorkerIdNumber == 0)
	{
		maxWorkerIdNumber = 63;
	}
	if (flake->WorkerId < 0 || flake->WorkerId > maxWorkerIdNumber)
	{
		perror("WorkerId error. (range:[0, {2^WorkerIdBitLength-1]}");
		exit(1);
	}

	// 4.SeqBitLength
	if (flake->SeqBitLength < 2 || flake->SeqBitLength > 21)
	{
		perror("SeqBitLength error. (range:[2, 21])");
		exit(1);
	}
	else
	{
		flake->SeqBitLength = flake->SeqBitLength <= 0 ? 6 : flake->SeqBitLength;
	}

	// 5.MaxSeqNumber
	uint32_t maxSeqNumber = (1 << flake->SeqBitLength) - 1;
	if (maxSeqNumber == 0)
	{
		maxSeqNumber = 63;
	}
	if (flake->MaxSeqNumber > maxSeqNumber)
	{
		perror("MaxSeqNumber error. (range:[1, {2^SeqBitLength-1}]");
		exit(1);
	}
	else
	{
		flake->MaxSeqNumber = flake->MaxSeqNumber <= 0 ? maxSeqNumber : flake->MaxSeqNumber;
	}

	// 6.MinSeqNumber
	if (flake->MinSeqNumber < 5 || flake->MinSeqNumber > maxSeqNumber)
	{
		perror("MinSeqNumber error. (range:[5, {MinSeqNumber}]");
		exit(1);
	}
	else
	{
		flake->MinSeqNumber = flake->MinSeqNumber <= 0 ? 5 : flake->MinSeqNumber;
	}

	// 7.Others
	flake->TopOverCostCount = flake->TopOverCostCount <= 0 ? 2000 : flake->TopOverCostCount;
	flake->_TimestampShift = flake->WorkerIdBitLength + flake->SeqBitLength;
	flake->_CurrentSeqNumber = flake->MinSeqNumber;
	flake->Method = flake->Method;
}

// static inline void EndOverCostAction(uint64_t useTimeTick, snowflake *flake)
// {
//   if (flake->_TermIndex > 10000)
//   {
//     flake->_TermIndex = 0;
//   }
// }

static inline uint64_t NextOverCostId(snowflake *flake)
{
	uint64_t currentTimeTick = GetCurrentTimeTick(flake);
	if (currentTimeTick > flake->_LastTimeTick)
	{
		// EndOverCostAction(currentTimeTick, flake);
		flake->_LastTimeTick = currentTimeTick;
		flake->_CurrentSeqNumber = flake->MinSeqNumber;
		flake->_IsOverCost = 0;
		flake->_OverCostCountInOneTerm = 0;
		// flake->_GenCountInOneTerm = 0;
		return CalcId(flake);
	}
	if (flake->_OverCostCountInOneTerm > flake->TopOverCostCount)
	{
		// EndOverCostAction(currentTimeTick, flake);
		flake->_LastTimeTick = GetNextTimeTick(flake);
		flake->_CurrentSeqNumber = flake->MinSeqNumber;
		flake->_IsOverCost = 0;
		flake->_OverCostCountInOneTerm = 0;
		// flake->_GenCountInOneTerm = 0;
		return CalcId(flake);
	}
	if (flake->_CurrentSeqNumber > flake->MaxSeqNumber)
	{
		flake->_LastTimeTick++;
		flake->_CurrentSeqNumber = flake->MinSeqNumber;
		flake->_IsOverCost = 1;
		flake->_OverCostCountInOneTerm++;
		// flake->_GenCountInOneTerm++;
		return CalcId(flake);
	}

	// flake->_GenCountInOneTerm++;
	return CalcId(flake);
}

static inline uint64_t NextNormalId(snowflake *flake)
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
		// flake->_TermIndex++;
		flake->_LastTimeTick++;
		flake->_CurrentSeqNumber = flake->MinSeqNumber;
		flake->_IsOverCost = 1;
		flake->_OverCostCountInOneTerm = 1;
		// flake->_GenCountInOneTerm = 1;
		return CalcId(flake);
	}
	return CalcId(flake);
}

static inline uint64_t GetSysCurrentTime()
{
#ifdef WIN32
	FILETIME file_time;
	GetSystemTimeAsFileTime(&file_time);
	uint64_t time = ((uint64_t)file_time.dwLowDateTime) + ((uint64_t)file_time.dwHighDateTime << 32);
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);
	return (uint64_t)((time - EPOCH) / 10000LL);
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	return (uint64_t)(t.tv_sec * 1000 + t.tv_usec / 1000);
#endif
}

static inline uint64_t GetCurrentTimeTick(snowflake *flake)
{
	return GetSysCurrentTime() - flake->BaseTime;
}

static inline uint64_t GetNextTimeTick(snowflake *flake)
{
	uint64_t tempTimeTicker = GetCurrentTimeTick(flake);
	struct timespec delay;
	delay.tv_sec = 0;
	delay.tv_nsec = 500000;
	while (1)
	{
		tempTimeTicker = GetCurrentTimeTick(flake);
		if (tempTimeTicker > flake->_LastTimeTick)
		{
			break;
		}
#ifdef WIN32
		SwitchToThread();
#else
		nanosleep(&delay, NULL);
#endif
	}
	return tempTimeTicker;
}

static inline uint64_t CalcId(snowflake *flake)
{
	uint64_t result = (flake->_LastTimeTick << flake->_TimestampShift) + (flake->WorkerId << flake->SeqBitLength) + (flake->_CurrentSeqNumber);
	flake->_CurrentSeqNumber++;
	return result;
}

static inline uint64_t CalcTurnBackId(snowflake *flake)
{
	uint64_t result = (flake->_TurnBackTimeTick << flake->_TimestampShift) + (flake->WorkerId << flake->SeqBitLength) + (flake->_TurnBackIndex );
	flake->_TurnBackTimeTick--;
	return result;
}

static inline uint64_t NextSonwId(snowflake *flake)
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

static inline uint64_t GetId(snowflake *flake)
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
	uint32_t i;
	for (i = 0; i < num; i++)
	{
		arr[i] = GetId(flake);
	}
	spin_unlock(&flake->_Lock, pid);
	return arr;
}
