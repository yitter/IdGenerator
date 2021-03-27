/*
 * 版权属于：yitter(yitter@126.com)
 * 代码翻译：amuluowin
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <stdbool.h>
#include "IdGenOptions.h"


extern pthread_mutex_t ThreadMutex;

typedef struct SnowFlakeWorker {
    uint8_t Method;
    uint64_t BaseTime;
    uint32_t WorkerId;
    uint8_t WorkerIdBitLength;
    uint8_t SeqBitLength;
    uint32_t MaxSeqNumber;
    uint32_t MinSeqNumber;
    uint32_t TopOverCostCount;

    uint8_t _TimestampShift;
    uint32_t _CurrentSeqNumber;
    int64_t _LastTimeTick;
    int64_t _TurnBackTimeTick;
    uint8_t _TurnBackIndex;
    bool _IsOverCost;
    uint32_t _OverCostCountInOneTerm;
    uint32_t _GenCountInOneTerm;
    uint32_t _TermIndex;

} SnowFlakeWorker;


extern SnowFlakeWorker *NewSnowFlakeWorker();

extern uint64_t WorkerM1NextId(SnowFlakeWorker *worker);

extern uint64_t GetCurrentTimeTick(SnowFlakeWorker *worker);

extern uint64_t GetNextTimeTick(SnowFlakeWorker *worker);

extern uint64_t GetCurrentTime();

extern uint64_t GetCurrentMicroTime();

