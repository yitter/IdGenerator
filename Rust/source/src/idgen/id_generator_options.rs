/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */

pub struct IdGeneratorOptions {
    /// 雪花计算方法,（1-漂移算法|2-传统算法），默认1
    pub Method: u8,

    /// 基础时间（ms单位），不能超过当前系统时间
    pub BaseTime: i64,

    /// 必须由外部设定，最大值 2^WorkerIdBitLength-1
    pub WorkerId: u32,

    /// 默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
    pub WorkerIdBitLength: u8,

    /// 默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
    pub SeqBitLength: u8,

    /// 设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
    pub MaxSeqNumber: u32,

    /// 默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
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