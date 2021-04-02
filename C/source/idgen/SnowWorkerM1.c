/*
 * 版权属于：yitter(yitter@126.com)
 * 代码翻译：amuluowin
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include "SnowWorkerM1.h"


pthread_mutex_t ThreadMutex = PTHREAD_MUTEX_INITIALIZER;

static void EndOverCostAction(uint64_t useTimeTick, SnowFlakeWorker *worker);

static uint64_t NextOverCostId(SnowFlakeWorker *worker);

static uint64_t NextNormalId(SnowFlakeWorker *worker);

static uint64_t CalcId(SnowFlakeWorker *worker);

static uint64_t CalcTurnBackId(SnowFlakeWorker *worker);


static inline void EndOverCostAction(uint64_t useTimeTick, SnowFlakeWorker *worker) {
    if (worker->_TermIndex > 10000) {
        worker->_TermIndex = 0;
    }
}

static inline uint64_t NextOverCostId(SnowFlakeWorker *worker) {
    uint64_t currentTimeTick = GetCurrentTimeTick(worker);
    if (currentTimeTick > worker->_LastTimeTick) {
        EndOverCostAction(currentTimeTick, worker);
        worker->_LastTimeTick = currentTimeTick;
        worker->_CurrentSeqNumber = worker->MinSeqNumber;
        worker->_IsOverCost = false;
        worker->_OverCostCountInOneTerm = 0;
        worker->_GenCountInOneTerm = 0;
        return CalcId(worker);
    }
    if (worker->_OverCostCountInOneTerm > worker->TopOverCostCount) {
        EndOverCostAction(currentTimeTick, worker);
        worker->_LastTimeTick = GetNextTimeTick(worker);
        worker->_CurrentSeqNumber = worker->MinSeqNumber;
        worker->_IsOverCost = false;
        worker->_OverCostCountInOneTerm = 0;
        worker->_GenCountInOneTerm = 0;
        return CalcId(worker);
    }
    if (worker->_CurrentSeqNumber > worker->MaxSeqNumber) {
        worker->_LastTimeTick++;
        worker->_CurrentSeqNumber = worker->MinSeqNumber;
        worker->_IsOverCost = true;
        worker->_OverCostCountInOneTerm++;
        worker->_GenCountInOneTerm++;
        return CalcId(worker);
    }

    worker->_GenCountInOneTerm++;
    return CalcId(worker);
}

static inline uint64_t NextNormalId(SnowFlakeWorker *worker) {
    uint64_t currentTimeTick = GetCurrentTimeTick(worker);
    if (currentTimeTick < worker->_LastTimeTick) {
        if (worker->_TurnBackTimeTick < 1) {
            worker->_TurnBackTimeTick = worker->_LastTimeTick - 1;
            worker->_TurnBackIndex++;
            if (worker->_TurnBackIndex > 4) {
                worker->_TurnBackIndex = 1;
            }
        }

        // usleep(1000); // 暂停1ms
        return CalcTurnBackId(worker);
    }

    if (worker->_TurnBackTimeTick > 0) {
        worker->_TurnBackTimeTick = 0;
    }

    if (currentTimeTick > worker->_LastTimeTick) {
        worker->_LastTimeTick = currentTimeTick;
        worker->_CurrentSeqNumber = worker->MinSeqNumber;
        return CalcId(worker);
    }

    if (worker->_CurrentSeqNumber > worker->MaxSeqNumber) {
        worker->_TermIndex++;
        worker->_LastTimeTick++;
        worker->_CurrentSeqNumber = worker->MinSeqNumber;
        worker->_IsOverCost = true;
        worker->_OverCostCountInOneTerm = 1;
        worker->_GenCountInOneTerm = 1;
        return CalcId(worker);
    }

    return CalcId(worker);
}

static inline uint64_t CalcId(SnowFlakeWorker *worker) {
    uint64_t result = (worker->_LastTimeTick << worker->_TimestampShift) | (worker->WorkerId << worker->SeqBitLength) |
                      (worker->_CurrentSeqNumber);
    worker->_CurrentSeqNumber++;
    return result;
}

static inline uint64_t CalcTurnBackId(SnowFlakeWorker *worker) {
    uint64_t result = (worker->_LastTimeTick << worker->_TimestampShift) | (worker->WorkerId << worker->SeqBitLength) |
                      (worker->_TurnBackTimeTick);
    worker->_TurnBackTimeTick--;
    return result;
}


extern SnowFlakeWorker *NewSnowFlakeWorker() {
    SnowFlakeWorker *worker = (SnowFlakeWorker *) malloc(sizeof(SnowFlakeWorker));
    worker->_IsOverCost = false;
    worker->_LastTimeTick = 0;
    worker->_TurnBackTimeTick = 0;
    worker->_TurnBackIndex = 0;
    worker->_OverCostCountInOneTerm = 0;
    worker->_GenCountInOneTerm = 0;
    worker->_TermIndex = 0;

    return worker;
}

extern uint64_t WorkerM1NextId(SnowFlakeWorker *worker) {
    pthread_mutex_lock(&ThreadMutex);
    uint64_t id = worker->_IsOverCost ? NextOverCostId(worker) : NextNormalId(worker);
    pthread_mutex_unlock(&ThreadMutex);
    return id;
}

extern uint64_t GetCurrentTimeTick(SnowFlakeWorker *worker) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t) tv.tv_sec * 1000 + tv.tv_usec / 1000 - worker->BaseTime);
}

extern uint64_t GetCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t) (tv.tv_sec)) * 1000 + tv.tv_usec / 1000;

    //static struct timeb t1;
    //    ftime(&t1);
    //    return (uint64_t) ((t1.time * 1000 + t1.millitm));
}

extern uint64_t GetCurrentMicroTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((uint64_t) tv.tv_sec * 1000000 + tv.tv_usec);
}

extern uint64_t GetNextTimeTick(SnowFlakeWorker *worker) {
    uint64_t tempTimeTicker = GetCurrentTimeTick(worker);
    while (tempTimeTicker <= worker->_LastTimeTick) {
        tempTimeTicker = GetCurrentTimeTick(worker);
    }
    return tempTimeTicker;
}
