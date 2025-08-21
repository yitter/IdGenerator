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

        /// <summary>
        /// 新增方法：根据传入时间生成一个新的 ID
        /// 如果 isMillisecondPrecision 为 false，则用当前时间的毫秒覆盖传入时间的毫秒
        /// </summary>
        /// <param name="dateTime"></param>
        /// <param name="isMillisecondPrecision"></param>
        /// <returns></returns>
        /// <exception cref="ArgumentOutOfRangeException"></exception>
        public override long NextId(DateTime dateTime, bool isMillisecondPrecision)
        {
            // 1. 统一时间到 UTC
            var dtUtc = dateTime.Kind == DateTimeKind.Utc ? dateTime : dateTime.ToUniversalTime();

            // 2. 如果没有毫秒精度，用当前毫秒覆盖
            if (!isMillisecondPrecision)
            {
                int currentMs = DateTime.UtcNow.Millisecond; // 0..999
                dtUtc = new DateTime(
                    dtUtc.Year, dtUtc.Month, dtUtc.Day,
                    dtUtc.Hour, dtUtc.Minute, dtUtc.Second,
                    currentMs,
                    DateTimeKind.Utc);
            }

            // 3. 计算与 BaseTime 的时间差（毫秒或秒）
            long timeTick;
            if (TimestampType == 0)
            {
                // 毫秒级时间戳
                timeTick = (long)(dtUtc - BaseTime).TotalMilliseconds;
            }
            else
            {
                // 秒级时间戳
                timeTick = (long)(dtUtc - BaseTime).TotalSeconds;
            }

            // 如果时间差为负，说明传入时间早于 BaseTime，按需求可以抛异常或调整，这里抛异常
            if (timeTick < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(dateTime), "指定时间早于 BaseTime，无法生成 ID。");
            }

            // 4. 生成独立序列号：取序列空间上半段，避免与实时发号序列冲突
            long half = (MaxSeqNumber + 1) / 2;
            long localSeq = Interlocked.Increment(ref _historySeqCounter);
            long seq = half + (localSeq & (half - 1));
            if (seq > MaxSeqNumber) seq &= MaxSeqNumber;

            // 5. 拼装 ID：时间戳部分左移 _TimestampShift 位，随后是 DataCenterId 和 WorkerId
            //    与 CalcId 的实现保持一致，但不修改任何实时状态
            long id = (timeTick << _TimestampShift)
                    + ((long)DataCenterId << DataCenterIdBitLength)
                    + ((long)WorkerId << SeqBitLength)
                    + seq;

            return id;
        }

    }
}
