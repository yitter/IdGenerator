/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
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
    /// 雪花漂移算法
    /// </summary> 
    internal class SnowWorkerM1 : ISnowWorker
    {
        /// <summary>
        /// 基础时间
        /// </summary>
        protected readonly DateTime StartTimeUtc = new DateTime(2020, 2, 20, 2, 20, 2, 20, DateTimeKind.Utc);

        /// <summary>
        /// 机器码
        /// </summary>
        protected readonly ushort WorkerId = 0;

        /// <summary>
        /// 机器码位长
        /// （机器码+序列数<=22位）
        /// </summary>
        protected readonly byte WorkerIdBitLength = 0;

        /// <summary>
        /// 自增序列数位长
        /// （机器码+序列数<=22位）
        /// </summary>
        protected readonly byte SeqBitLength = 0;

        /// <summary>
        /// 最大序列数（含此值）
        /// 超过最大值，就会从MinSeqNumber开始
        /// </summary>
        protected readonly int MaxSeqNumber = 0;

        /// <summary>
        /// 最小序列数（含此值）
        /// </summary>
        protected readonly ushort MinSeqNumber = 0;

        /// <summary>
        /// 最大漂移次数
        /// </summary>
        protected readonly int TopOverCostCount = 0;

        protected readonly byte _TimestampShift = 0;
        protected static object _SyncLock = new object();

        protected ushort _CurrentSeqNumber;
        protected long _LastTimeTick = -1L;
        protected long _TurnBackTimeTick = -1L;

        protected bool _IsOverCost = false;
        protected int _OverCostCountInOneTerm = 0;
        protected int _GenCountInOneTerm = 0;
        protected int _TermIndex = 0;
        public Action<OverCostActionArg> GenAction { get; set; }

        public SnowWorkerM1(IdGeneratorOptions options)
        {
            WorkerId = options.WorkerId;
            WorkerIdBitLength = options.WorkerIdBitLength;
            SeqBitLength = options.SeqBitLength;
            MaxSeqNumber = options.MaxSeqNumber;
            MinSeqNumber = options.MinSeqNumber;
            _CurrentSeqNumber = options.MinSeqNumber;
            TopOverCostCount = options.TopOverCostCount;

            if (options.StartTime != DateTime.MinValue)
            {
                StartTimeUtc = options.StartTime;
            }

            if (WorkerId < 1)
            {
                WorkerId = (ushort)DateTime.Now.Millisecond;
            }

            if (SeqBitLength == 0)
            {
                SeqBitLength = 10;
            }

            if (WorkerIdBitLength == 0)
            {
                WorkerIdBitLength = 10;
            }

            if (MaxSeqNumber == 0)
            {
                MaxSeqNumber = (int)Math.Pow(2, SeqBitLength);
            }

            _TimestampShift = (byte)(WorkerIdBitLength + SeqBitLength);
        }


        private void DoGenIdAction(OverCostActionArg arg)
        {
            Task.Run(() =>
            {
                GenAction(arg);
            });
        }

        private void BeginOverCostCallBack(in long useTimeTick)
        {
            if (GenAction == null)
            {
                return;
            }

            DoGenIdAction(new OverCostActionArg(
                WorkerId,
                useTimeTick,
                1,
                _OverCostCountInOneTerm,
                _GenCountInOneTerm,
                _TermIndex));
        }

        private void EndOverCostCallBack(in long useTimeTick)
        {
            if (_TermIndex > 10000)
            {
                _TermIndex = 0;
            }

            if (GenAction == null)
            {
                return;
            }

            DoGenIdAction(new OverCostActionArg(
                WorkerId,
                useTimeTick,
                2,
                _OverCostCountInOneTerm,
                _GenCountInOneTerm,
                _TermIndex));
        }

        private void TurnBackCallBack(in long useTimeTick)
        {
            if (GenAction == null)
            {
                return;
            }

            DoGenIdAction(new OverCostActionArg(
            WorkerId,
            useTimeTick,
            8,
            _OverCostCountInOneTerm,
            _GenCountInOneTerm,
            _TermIndex));
        }

        private long NextOverCostId()
        {
            long currentTimeTick = GetCurrentTimeTick();

            if (currentTimeTick > _LastTimeTick)
            {
                EndOverCostCallBack(currentTimeTick);

                _LastTimeTick = currentTimeTick;
                _CurrentSeqNumber = MinSeqNumber;
                _IsOverCost = false;
                _OverCostCountInOneTerm = 0;
                _GenCountInOneTerm = 0;

                return CalcId(_LastTimeTick);
            }

            if (_OverCostCountInOneTerm >= TopOverCostCount)
            {
                EndOverCostCallBack(currentTimeTick);

                _LastTimeTick = GetNextTimeTick();
                _CurrentSeqNumber = MinSeqNumber;
                _IsOverCost = false;
                _OverCostCountInOneTerm = 0;
                _GenCountInOneTerm = 0;

                return CalcId(_LastTimeTick);
            }

            if (_CurrentSeqNumber > MaxSeqNumber)
            {
                _LastTimeTick++;
                _CurrentSeqNumber = MinSeqNumber;
                _IsOverCost = true;
                _OverCostCountInOneTerm++;
                _GenCountInOneTerm++;

                return CalcId(_LastTimeTick);
            }

            _GenCountInOneTerm++;
            return CalcId(_LastTimeTick);
        }

        private long NextNormalId()
        {
            long currentTimeTick = GetCurrentTimeTick();

            if (currentTimeTick > _LastTimeTick)
            {
                _LastTimeTick = currentTimeTick;
                _CurrentSeqNumber = MinSeqNumber;

                return CalcId(_LastTimeTick);
            }

            if (_CurrentSeqNumber > MaxSeqNumber)
            {
                BeginOverCostCallBack(currentTimeTick);

                _TermIndex++;
                _LastTimeTick++;
                _CurrentSeqNumber = MinSeqNumber;
                _IsOverCost = true;
                _OverCostCountInOneTerm++;
                _GenCountInOneTerm = 1;

                return CalcId(_LastTimeTick);
            }

            if (currentTimeTick < _LastTimeTick)
            {
                if (_TurnBackTimeTick < 1)
                {
                    _TurnBackTimeTick = _LastTimeTick - 1;
                }

                Thread.Sleep(10);
                TurnBackCallBack(_TurnBackTimeTick);

                return CalcTurnBackId(_TurnBackTimeTick);
            }


            return CalcId(_LastTimeTick);
        }

        private long CalcId(in long useTimeTick)
        {
            var result = ((useTimeTick << _TimestampShift) +
                ((long)WorkerId << SeqBitLength) +
                (uint)_CurrentSeqNumber);

            _CurrentSeqNumber++;
            return result;
        }

        private long CalcTurnBackId(in long useTimeTick)
        {
            var result = ((useTimeTick << _TimestampShift) +
                ((long)WorkerId << SeqBitLength) + 0);

            _TurnBackTimeTick--;
            return result;
        }

        protected virtual long GetCurrentTimeTick()
        {
            return (long)(DateTime.UtcNow - StartTimeUtc).TotalMilliseconds;
        }

        protected virtual long GetNextTimeTick()
        {
            long tempTimeTicker = GetCurrentTimeTick();

            while (tempTimeTicker <= _LastTimeTick)
            {
                tempTimeTicker = GetCurrentTimeTick();
            }

            return tempTimeTicker;
        }


        public virtual long NextId()
        {
            lock (_SyncLock)
            {
                return _IsOverCost ? NextOverCostId() : NextNormalId();
            }
        }
    }
}
