using System;
using System.Collections.Generic;
using System.Text;

namespace Yitter.IdGenerator
{
    public class IdGeneratorOptions
    {
        /// <summary>
        /// 雪花计算方法
        /// （1|2）
        /// </summary>
        public short Method { get; set; } = 1;

        /// <summary>
        /// 开始时间（UTC格式）
        /// 不能超过当前系统时间
        /// </summary>
        public DateTime StartTime { get; set; } = DateTime.MinValue;

        /// <summary>
        /// 机器码
        /// 与 WorkerIdBitLength 有关系
        /// </summary>
        public ushort WorkerId { get; set; } = 0;

        /// <summary>
        /// 机器码位长
        /// 范围：2-21（要求：序列数位长+机器码位长不超过22）。
        /// 建议范围：6-12。
        /// </summary>
        public byte WorkerIdBitLength { get; set; } = 6;//10;

        /// <summary>
        /// 序列数位长
        /// 范围：2-21（要求：序列数位长+机器码位长不超过22）。
        /// 建议范围：6-14。
        /// </summary>
        public byte SeqBitLength { get; set; } = 6;//10;

        /// <summary>
        /// 最大序列数（含）
        /// （由SeqBitLength计算的最大值）
        /// </summary>
        public int MaxSeqNumber { get; set; } = 0;

        /// <summary>
        /// 最小序列数（含）
        /// 默认11，不小于5，不大于MaxSeqNumber-2
        /// </summary>
        public ushort MinSeqNumber { get; set; } = 11;

        /// <summary>
        /// 最大漂移次数（含），
        /// 默认2000，推荐范围500-10000（与计算能力有关）
        /// </summary>
        public int TopOverCostCount { get; set; } = 2000;


        public IdGeneratorOptions()
        {

        }
    }
}
