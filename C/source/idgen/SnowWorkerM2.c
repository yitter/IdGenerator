/*
 * 版权属于：yitter(yitter@126.com)
 * 代码翻译：amuluowin
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include "SnowWorkerM2.h"


extern uint64_t WorkerM2NextId(SnowFlakeWorker *worker) {
    pthread_mutex_lock(&ThreadMutex);
    uint64_t currentTimeTick = GetCurrentTimeTick(worker);
    if (worker->_LastTimeTick == currentTimeTick) {
        worker->_CurrentSeqNumber = (++worker->_CurrentSeqNumber) & worker->MaxSeqNumber;
        if (worker->_CurrentSeqNumber == 0) {
            currentTimeTick = GetNextTimeTick(worker);
        }
    } else {
        worker->_CurrentSeqNumber = worker->MinSeqNumber;
    }
    worker->_LastTimeTick = currentTimeTick;
    uint64_t id = (uint64_t) ((currentTimeTick << worker->_TimestampShift) |
                              (worker->WorkerId << worker->SeqBitLength) |
                              worker->_CurrentSeqNumber);
    pthread_mutex_unlock(&ThreadMutex);
    return id;
}
