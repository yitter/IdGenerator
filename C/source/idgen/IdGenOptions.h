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
    /// 机器码，与 WorkerIdBitLength 有关系
    uint32_t WorkerId;
    /// 机器码位长，范围：1-21（要求：序列数位长+机器码位长不超过22）
    uint8_t WorkerIdBitLength;
    /// 序列数位长，范围：2-21（要求：序列数位长+机器码位长不超过22）
    uint8_t SeqBitLength;
    /// 最大序列数（含），（由 SeqBitLength 计算的最大值）
    uint32_t MaxSeqNumber;
    /// 最小序列数（含），默认5，不小于5，不大于 MaxSeqNumber
    uint32_t MinSeqNumber;
    /// 最大漂移次数（含），默认2000，推荐范围 500-20000（与计算能力有关）
    uint32_t TopOverCostCount;

} IdGeneratorOptions;

extern IdGeneratorOptions BuildIdGenOptions(uint32_t workerId);

