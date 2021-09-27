/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
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
        /// 必须由外部设定，最大值 2^WorkerIdBitLength-1
        /// </summary>
        public virtual ushort WorkerId { get; set; } = 0;

        /// <summary>
        /// 机器码位长
        /// 默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
        /// </summary>
        public virtual byte WorkerIdBitLength { get; set; } = 6;//10;

        /// <summary>
        /// 序列数位长
        /// 默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
        /// </summary>
        public virtual byte SeqBitLength { get; set; } = 6;//10;

        /// <summary>
        /// 最大序列数（含）
        /// 设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
        /// </summary>
        public virtual int MaxSeqNumber { get; set; } = 0;

        /// <summary>
        /// 最小序列数（含）
        /// 默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
        /// </summary>
        public virtual ushort MinSeqNumber { get; set; } = 5;

        /// <summary>
        /// 最大漂移次数（含），
        /// 默认2000，推荐范围500-10000（与计算能力有关）
        /// </summary>
        public virtual int TopOverCostCount { get; set; } = 2000;


        /// <summary>
        /// 数据中心ID（默认0）
        /// </summary>
        public virtual uint DataCenterId { get; set; } = 0;

        /// <summary>
        /// 数据中心ID长度（默认0）
        /// </summary>
        public virtual byte DataCenterIdBitLength { get; set; } = 0;

        /// <summary>
        /// 时间戳类型（0-毫秒，1-秒），默认0
        /// </summary>
        public virtual byte TimestampType { get; set; } = 0;


        public IdGeneratorOptions()
        {

        }

        public IdGeneratorOptions(ushort workerId)
        {
            WorkerId = workerId;
        }
    }
}
