/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.yitter.contract;

/**
 * 雪花算法使用的参数
 * 此处代码不采用 get/set 那种冗长的写法
 */
public class IdGeneratorOptions {

    /**
     * 雪花计算方法
     * （1-漂移算法|2-传统算法），默认1
     */
    public short Method = 1;

    /**
     * 开始时间
     * 不能超过当前系统时间
     */
    public long BaseTime = 1582136402000L;

    /**
     * 机器码，必须由外部系统设置
     * 与 WorkerIdBitLength 有关系
     * （short类型，最大值32766，如果有更高要求，请修改数据类型，或联系作者)
     */
    public short WorkerId = 0;

    /**
     * 机器码位长
     * 范围：1-21（要求：序列数位长+机器码位长不超过22）。
     * 建议范围：6-12。
     */
    public byte WorkerIdBitLength = 6;

    /**
     * 序列数位长
     * 范围：2-21（要求：序列数位长+机器码位长不超过22）。
     * 建议范围：6-14。
     */
    public byte SeqBitLength = 6;

    /**
     * 最大序列数（含）
     * （由SeqBitLength计算的最大值）
     */
    public short MaxSeqNumber = 0;

    /**
     * 最小序列数（含）
     * 默认5，不小于1，不大于MaxSeqNumber
     */
    public short MinSeqNumber = 5;

    /**
     * 最大漂移次数（含）
     * 默认2000，推荐范围500-10000（与计算能力有关）
     */
    public short TopOverCostCount = 2000;

    public IdGeneratorOptions() {

    }

    public IdGeneratorOptions(short workerId) {
        WorkerId = workerId;
    }
}
