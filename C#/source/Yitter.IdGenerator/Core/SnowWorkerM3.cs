/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 * 版权协议：MIT
 * 版权说明：只要保留本版权，你可以免费使用、修改、分发本代码。
 * 免责条款：任何因为本代码产生的系统、法律、政治、宗教问题，均与版权所有者无关。
 * 
 */

using System;
using System.Threading;
using System.Threading.Tasks;

namespace Yitter.IdGenerator
{
    /// <summary>
    /// 雪花漂移算法（支持数据中心ID和秒级时间戳）
    /// </summary> 
    internal class SnowWorkerM3 : SnowWorkerM1
    {
        /// <summary>
        /// 数据中心ID（默认0）
        /// </summary>
        protected readonly uint DataCenterId = 0;

        /// <summary>
        /// 数据中心ID长度（默认0）
        /// </summary>
        protected readonly byte DataCenterIdBitLength = 0;

        /// <summary>
        /// 时间戳类型（0-毫秒，1-秒），默认0
        /// </summary>
        protected readonly byte TimestampType = 0;


        public SnowWorkerM3(IdGeneratorOptions options) : base(options)
        {
            // 秒级时间戳类型
            TimestampType = options.TimestampType;

            // DataCenter相关
            DataCenterId = options.DataCenterId;
            DataCenterIdBitLength = options.DataCenterIdBitLength;

            if (TimestampType == 1)
            {
                TopOverCostCount = 0;
            }
            _TimestampShift = (byte)(DataCenterIdBitLength + WorkerIdBitLength + SeqBitLength);
        }

        protected override long CalcId(long useTimeTick)
        {
            var result = ((useTimeTick << _TimestampShift) +
                ((long)DataCenterId << DataCenterIdBitLength) +
                ((long)WorkerId << SeqBitLength) +
                (long)_CurrentSeqNumber);

            _CurrentSeqNumber++;
            return result;
        }

        protected override long CalcTurnBackId(long useTimeTick)
        {
            var result = ((useTimeTick << _TimestampShift) +
                ((long)DataCenterId << DataCenterIdBitLength) +
                ((long)WorkerId << SeqBitLength) +
                _TurnBackIndex);

            _TurnBackTimeTick--;
            return result;
        }

        protected override long GetCurrentTimeTick()
        {
            return TimestampType == 0 ?
                (long)(DateTime.UtcNow - BaseTime).TotalMilliseconds :
                (long)(DateTime.UtcNow - BaseTime).TotalSeconds;
        }

    }
}
