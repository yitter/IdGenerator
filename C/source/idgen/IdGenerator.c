/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 */
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "IdGenerator.h"

static inline uint64_t WorkerM1Id()
{
    return WorkerM1NextId(_idGenerator->Worker);
}

static inline uint64_t WorkerM2Id()
{
    return WorkerM2NextId(_idGenerator->Worker);
}

extern IdGenerator *GetIdGenInstance()
{
    if (_idGenerator != NULL)
        return _idGenerator;
    else
    {
        _idGenerator = (IdGenerator *)malloc(sizeof(IdGenerator));
        _idGenerator->Worker = NewSnowFlakeWorker();
        return _idGenerator;
    }
}

extern void SetOptions(IdGeneratorOptions options)
{
    if (GetIdGenInstance() == NULL)
    {
        exit(1);
    }

    // 1.BaseTime
    if (options.BaseTime == 0)
    {
        _idGenerator->Worker->BaseTime = 1582136402000;
    }
    else if (options.BaseTime < 631123200000 || options.BaseTime > GetCurrentTime())
    {
        perror("BaseTime error.");
        exit(1);
    }
    else
    {
        _idGenerator->Worker->BaseTime = options.BaseTime;
    }

    // 2.WorkerIdBitLength
    if (options.WorkerIdBitLength <= 0)
    {
        perror("WorkerIdBitLength error.(range:[1, 21])");
        exit(1);
    }
    if (options.SeqBitLength + options.WorkerIdBitLength > 22)
    {
        perror("error：WorkerIdBitLength + SeqBitLength <= 22");
        exit(1);
    }
    else
    {
        // _idGenerator->Worker->WorkerIdBitLength = options.WorkerIdBitLength;
        _idGenerator->Worker->WorkerIdBitLength = options.WorkerIdBitLength <= 0 ? 6 : options.WorkerIdBitLength;
    }

    // 3.WorkerId
    uint32_t maxWorkerIdNumber = (1 << options.WorkerIdBitLength) - 1;
    if (maxWorkerIdNumber == 0)
    {
        maxWorkerIdNumber = 63;
    }
    if (options.WorkerId < 0 || options.WorkerId > maxWorkerIdNumber)
    {
        perror("WorkerId error. (range:[0, {2^options.WorkerIdBitLength-1]}");
        exit(1);
    }
    else
    {
        _idGenerator->Worker->WorkerId = options.WorkerId;
    }

    // 4.SeqBitLength
    if (options.SeqBitLength < 2 || options.SeqBitLength > 21)
    {
        perror("SeqBitLength error. (range:[2, 21])");
        exit(1);
    }
    else
    {
        // _idGenerator->Worker->SeqBitLength = options.SeqBitLength;
        _idGenerator->Worker->SeqBitLength = options.SeqBitLength <= 0 ? 6 : options.SeqBitLength;
    }

    // 5.MaxSeqNumber
    uint32_t maxSeqNumber = (1 << options.SeqBitLength) - 1;
    if (maxSeqNumber == 0)
    {
        maxSeqNumber = 63;
    }
    if (options.MaxSeqNumber > maxSeqNumber)
    {
        perror("MaxSeqNumber error. (range:[1, {2^options.SeqBitLength-1}]");
        exit(1);
    }
    else
    {
        _idGenerator->Worker->MaxSeqNumber = options.MaxSeqNumber <= 0 ? maxSeqNumber : options.MaxSeqNumber;
    }

    // 6.MinSeqNumber
    if (options.MinSeqNumber < 5 || options.MinSeqNumber > maxSeqNumber)
    {
        perror("MinSeqNumber error. (range:[5, {options.MinSeqNumber}]");
        exit(1);
    }
    else
    {
        _idGenerator->Worker->MinSeqNumber = options.MinSeqNumber <= 0 ? 5 : options.MinSeqNumber;
    }

    // 7.TopOverCostCount
    if (options.TopOverCostCount < 0 || options.TopOverCostCount > 10000)
    {
        perror("TopOverCostCount error. (range:[0, 10000]");
        exit(1);
    }
    else
    {
        //_idGenerator->Worker->TopOverCostCount = options.TopOverCostCount <= 0 ? 2000 : options.TopOverCostCount;
        _idGenerator->Worker->TopOverCostCount = options.TopOverCostCount;
    }

    // 8.Others
    _idGenerator->Worker->_TimestampShift =
        _idGenerator->Worker->WorkerIdBitLength + _idGenerator->Worker->SeqBitLength;
    _idGenerator->Worker->_CurrentSeqNumber = _idGenerator->Worker->MinSeqNumber;

    _idGenerator->Worker->Method = options.Method;
    if (options.Method == 2)
    {
        _idGenerator->NextId = WorkerM2Id;
    }
    else
    {
        _idGenerator->NextId = WorkerM1Id;
        usleep(500 * 1000); // 暂停500ms
    }
}
