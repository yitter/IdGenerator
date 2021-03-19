/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 * 版权协议：MIT
 * 版权说明：只要保留本版权，你可以免费使用、修改、分发本代码。
 * 免责条款：任何因为本代码产生的系统、法律、政治、宗教问题，均与版权所有者无关。
 * 
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace Yitter.IdGenerator
{
    public class IdGeneratorOptions
    {
        /// <summary>
        /// 雪花计算方法
        /// （1-漂移算法|2-传统算法），默认1
        /// </summary>
        public virtual short Method { get; set; } = 1;

        /// <summary>
        /// 基础时间（UTC格式）
        /// 不能超过当前系统时间
        /// </summary>
        public virtual DateTime BaseTime { get; set; } = new DateTime(2020, 2, 20, 2, 20, 2, 20, DateTimeKind.Utc);

        /// <summary>
        /// 机器码
        /// 与 WorkerIdBitLength 有关系
        /// （ushort类型，最大值65535，如果有更高要求，请修改数据类型，或联系作者)
        /// </summary>
        public virtual ushort WorkerId { get; set; } = 0;

        /// <summary>
        /// 机器码位长
        /// 范围：1-21（要求：序列数位长+机器码位长不超过22）。
        /// 建议范围：6-12。
        /// </summary>
        public virtual byte WorkerIdBitLength { get; set; } = 6;//10;

        /// <summary>
        /// 序列数位长
        /// 范围：2-21（要求：序列数位长+机器码位长不超过22）。
        /// 建议范围：6-14。
        /// </summary>
        public virtual byte SeqBitLength { get; set; } = 6;//10;

        /// <summary>
        /// 最大序列数（含）
        /// （由SeqBitLength计算的最大值）
        /// </summary>
        public virtual int MaxSeqNumber { get; set; } = 0;

        /// <summary>
        /// 最小序列数（含）
        /// 默认5，不小于1，不大于MaxSeqNumber
        /// </summary>
        public virtual ushort MinSeqNumber { get; set; } = 5;

        /// <summary>
        /// 最大漂移次数（含），
        /// 默认2000，推荐范围500-10000（与计算能力有关）
        /// </summary>
        public virtual int TopOverCostCount { get; set; } = 2000;


        public IdGeneratorOptions()
        {

        }

        public IdGeneratorOptions(ushort workerId)
        {
            WorkerId = workerId;
        }
    }
}
