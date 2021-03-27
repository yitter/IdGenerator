/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "IdGenerator.h"


static inline uint64_t WorkerM1Id() {
    return WorkerM1NextId(_idGenerator->Worker);
}

static inline uint64_t WorkerM2Id() {
    return WorkerM2NextId(_idGenerator->Worker);
}


extern IdGenerator *GetIdGenInstance() {
    if (_idGenerator != NULL)
        return _idGenerator;
    else {
        _idGenerator = (IdGenerator *) malloc(sizeof(IdGenerator));
        _idGenerator->Worker = NewSnowFlakeWorker();
        return _idGenerator;
    }
}

extern void SetOptions(IdGeneratorOptions options) {
    if (GetIdGenInstance() == NULL) {
        exit(1);
    }

    // BaseTime
    if (options.BaseTime == 0) {
        _idGenerator->Worker->BaseTime = 1582136402000;
    } else if (options.BaseTime < 631123200000 || options.BaseTime > GetCurrentTime()) {
        perror("BaseTime error.");
        exit(1);
    } else {
        _idGenerator->Worker->BaseTime = options.BaseTime;
    }

    // WorkerIdBitLength
    if (options.WorkerIdBitLength <= 0) {
        perror("WorkerIdBitLength error.(range:[1, 21])");
        exit(1);
    }
    if (options.SeqBitLength + options.WorkerIdBitLength > 22) {
        perror("error：WorkerIdBitLength + SeqBitLength <= 22");
        exit(1);
    } else {
//        _idGenerator->Worker->WorkerIdBitLength = options.WorkerIdBitLength;
        _idGenerator->Worker->WorkerIdBitLength = options.WorkerIdBitLength <= 0 ? 6 : options.WorkerIdBitLength;
    }

    // WorkerId
    uint32_t maxWorkerIdNumber = (1 << options.WorkerIdBitLength) - 1;
    if (options.WorkerId < 0 || options.WorkerId > maxWorkerIdNumber) {
        perror("WorkerId error. (range:[0, {2^options.WorkerIdBitLength-1]}");
        exit(1);
    } else {
        _idGenerator->Worker->WorkerId = options.WorkerId;
    }

    // SeqBitLength
    if (options.SeqBitLength < 2 || options.SeqBitLength > 21) {
        perror("SeqBitLength error. (range:[2, 21])");
        exit(1);
    } else {
//        _idGenerator->Worker->SeqBitLength = options.SeqBitLength;
        _idGenerator->Worker->SeqBitLength = options.SeqBitLength <= 0 ? 6 : options.SeqBitLength;
    }

    // MaxSeqNumber
    uint32_t maxSeqNumber = (1 << options.SeqBitLength) - 1;
    if (options.MaxSeqNumber > maxSeqNumber) {
        perror("MaxSeqNumber error. (range:[1, {2^options.SeqBitLength-1}]");
        exit(1);
    } else {
        _idGenerator->Worker->MaxSeqNumber = options.MaxSeqNumber <= 0 ? maxSeqNumber : options.MaxSeqNumber;
    }

    // MinSeqNumber
    if (options.MinSeqNumber > maxSeqNumber || options.MinSeqNumber < 5) {
        perror("MinSeqNumber error. (range:[5, {options.MinSeqNumber}]");
        exit(1);
    } else {
        _idGenerator->Worker->MinSeqNumber = options.MinSeqNumber <= 0 ? 5 : options.MinSeqNumber;
    }

    _idGenerator->Worker->TopOverCostCount = options.TopOverCostCount <= 0 ? 2000 : options.TopOverCostCount;
    _idGenerator->Worker->_TimestampShift = options.WorkerIdBitLength + options.SeqBitLength;
    _idGenerator->Worker->_CurrentSeqNumber = options.MinSeqNumber;

    _idGenerator->Worker->Method = options.Method;
    if (options.Method == 2) {
        _idGenerator->NextId = WorkerM2Id;
    } else {
        _idGenerator->NextId = WorkerM1Id;
        sleep(1);
    }
}

