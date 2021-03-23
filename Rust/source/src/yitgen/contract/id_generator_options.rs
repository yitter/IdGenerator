/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */

pub struct IdGeneratorOptions {
    /// 雪花计算方法,（1-漂移算法|2-传统算法），默认1
    pub Method: u8,
    /// 基础时间，不能超过当前系统时间
    pub BaseTime: i64,
    /// 机器码，与 WorkerIdBitLength 有关系
    pub WorkerId: u32,
    /// 机器码位长，范围：1-21（要求：序列数位长+机器码位长不超过22）
    pub WorkerIdBitLength: u8,
    /// 序列数位长，范围：2-21（要求：序列数位长+机器码位长不超过22）
    pub SeqBitLength: u8,
    /// 最大序列数（含），（由 SeqBitLength 计算的最大值）
    pub MaxSeqNumber: u32,
    /// 最小序列数（含），默认5，不小于1，不大于 MaxSeqNumber
    pub MinSeqNumber: u32,
    /// 最大漂移次数（含），默认2000，推荐范围 500-20000（与计算能力有关）
    pub TopOverCostCount: u32,
}

impl IdGeneratorOptions {
    pub fn New(workerId: u32) -> IdGeneratorOptions {
        return IdGeneratorOptions {
            Method: 1,
            WorkerId: workerId,
            BaseTime: 1582136402000,
            WorkerIdBitLength: 6,
            SeqBitLength: 6,
            MaxSeqNumber: 0,
            MinSeqNumber: 5,
            TopOverCostCount: 2000,
        };
    }
}