/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#pragma once

#include <stdint.h>


typedef struct IdGenOptions {
    /// 雪花计算方法,（1-漂移算法|2-传统算法），默认1
    uint8_t Method;

    /// 基础时间（ms单位），不能超过当前系统时间
    uint64_t BaseTime;

    /// 机器码，必须由外部设定，最大值 2^WorkerIdBitLength-1
    uint32_t WorkerId;

    /// 机器码位长，默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
    uint8_t WorkerIdBitLength;

    /// 序列数位长，默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
    uint8_t SeqBitLength;

    /// 最大序列数（含），设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
    uint32_t MaxSeqNumber;

    /// 最小序列数（含），默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号是0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
    uint32_t MinSeqNumber;

    /// 最大漂移次数（含），默认2000，推荐范围 500-20000（与计算能力有关）
    uint32_t TopOverCostCount;

} IdGeneratorOptions;

extern IdGeneratorOptions BuildIdGenOptions(uint32_t workerId);

