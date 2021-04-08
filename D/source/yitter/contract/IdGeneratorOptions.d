/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.contract.IdGeneratorOptions;

import std.datetime;

/**
 * 雪花算法使用的参数
 * 参数说明，参考 README.md 的 “配置参数” 章节。
 */
class IdGeneratorOptions {

    /**
     * 雪花计算方法
     * （1-漂移算法|2-传统算法），默认1
     */
    short Method = 1;

    /**
     * 基础时间（ms单位）
     * 不能超过当前系统时间
     */
    // long BaseTime = 1582136402000L;
    SysTime BaseTime;

    /**
     * 机器码
     * 必须由外部设定，最大值 2^WorkerIdBitLength-1
     */
    short WorkerId = 0;

    /**
     * 机器码位长
     * 默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
     */
    byte WorkerIdBitLength = 6;

    /**
     * 序列数位长
     * 默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
     */
    byte SeqBitLength = 6;

    /**
     * 最大序列数（含）
     * 设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
     */
    short MaxSeqNumber = 0;

    /**
     * 最小序列数（含）
     * 默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号是0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
     */
    short MinSeqNumber = 5;

    /**
     * 最大漂移次数（含）
     * 默认2000，推荐范围500-10000（与计算能力有关）
     */
    short TopOverCostCount = 2000;

    this() {
        BaseTime = SysTime(DateTime(2020, 2, 20, 2, 20, 2));
    }

    this(short workerId) {
        WorkerId = workerId;
        BaseTime = SysTime(DateTime(2020, 2, 20, 2, 20, 2));
    }
}
