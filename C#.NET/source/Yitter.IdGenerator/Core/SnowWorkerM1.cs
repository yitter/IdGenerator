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
        protected readonly DateTime BaseTime;

        /// <summary>
        /// 机器码
        /// </summary>
        protected readonly ushort WorkerId = 0;

        /// <summary>
        /// 机器码位长
        /// </summary>
        protected readonly byte WorkerIdBitLength = 0;

        /// <summary>
        /// 自增序列数位长
        /// </summary>
        protected readonly byte SeqBitLength = 0;

        /// <summary>
        /// 最大序列数（含）
        /// </summary>
        protected readonly int MaxSeqNumber = 0;

        /// <summary>
        /// 最小序列数（含）
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
        protected byte _TurnBackIndex = 0;

        protected bool _IsOverCost = false;
        protected int _OverCostCountInOneTerm = 0;
        protected int _GenCountInOneTerm = 0;
        protected int _TermIndex = 0;

        //private static long _StartTimeTick = 0;
        //private static long _BaseTimeTick = 0;

        public Action<OverCostActionArg> GenAction { get; set; }


        public SnowWorkerM1(IdGeneratorOptions options)
        {
            WorkerId = options.WorkerId;
            WorkerIdBitLength = options.WorkerIdBitLength;
            SeqBitLength = options.SeqBitLength;
            MaxSeqNumber = options.MaxSeqNumber;
            MinSeqNumber = options.MinSeqNumber;
            TopOverCostCount = options.TopOverCostCount;

            if (options.BaseTime != DateTime.MinValue)
            {
                BaseTime = options.BaseTime;
            }

            if (WorkerId < 1)
            {
                WorkerId = (ushort)DateTime.Now.Millisecond;
            }

            if (SeqBitLength == 0)
            {
                SeqBitLength = 6;
            }

            if (WorkerIdBitLength == 0)
            {
                WorkerIdBitLength = 6;
            }

            if (MaxSeqNumber == 0)
            {
                MaxSeqNumber = (int)Math.Pow(2, SeqBitLength) - 1;
            }

            _TimestampShift = (byte)(WorkerIdBitLength + SeqBitLength);
            _CurrentSeqNumber = options.MinSeqNumber;

            //_BaseTimeTick = BaseTime.Ticks;
            //_StartTimeTick = (long)(DateTime.UtcNow.Subtract(BaseTime).TotalMilliseconds) - Environment.TickCount;
        }


        private void DoGenIdAction(OverCostActionArg arg)
        {
            Task.Run(() =>
            {
                GenAction(arg);
            });
        }

        private void BeginOverCostAction(in long useTimeTick)
        {
            return;

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

        private void EndOverCostAction(in long useTimeTick)
        {
            if (_TermIndex > 10000)
            {
                _TermIndex = 0;
            }
            return;

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

        private void BeginTurnBackAction(in long useTimeTick)
        {
            return;

            if (GenAction == null)
            {
                return;
            }

            DoGenIdAction(new OverCostActionArg(
            WorkerId,
            useTimeTick,
            8,
            0,
            0,
            _TurnBackIndex));
        }

        private void EndTurnBackAction(in long useTimeTick)
        {
            return;

            if (GenAction == null)
            {
                return;
            }

            DoGenIdAction(new OverCostActionArg(
            WorkerId,
            useTimeTick,
            9,
            0,
            0,
            _TurnBackIndex));
        }

        private long NextOverCostId()
        {
            long currentTimeTick = GetCurrentTimeTick();

            if (currentTimeTick > _LastTimeTick)
            {
                EndOverCostAction(currentTimeTick);

                _LastTimeTick = currentTimeTick;
                _CurrentSeqNumber = MinSeqNumber;
                _IsOverCost = false;
                _OverCostCountInOneTerm = 0;
                _GenCountInOneTerm = 0;

                return CalcId(_LastTimeTick);
            }

            if (_OverCostCountInOneTerm >= TopOverCostCount)
            {
                EndOverCostAction(currentTimeTick);

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

            if (currentTimeTick < _LastTimeTick)
            {
                if (_TurnBackTimeTick < 1)
                {
                    _TurnBackTimeTick = _LastTimeTick - 1;
                    _TurnBackIndex++;

                    // 每毫秒序列数的前5位是预留位，0用于手工新值，1-4是时间回拨次序
                    // 最多4次回拨（防止回拨重叠）
                    if (_TurnBackIndex > 4)
                    {
                        _TurnBackIndex = 1;
                    }

                    BeginTurnBackAction(_TurnBackTimeTick);
                }

                Thread.Sleep(10);
                return CalcTurnBackId(_TurnBackTimeTick);
            }

            // 时间追平时，_TurnBackTimeTick清零
            if (_TurnBackTimeTick > 0)
            {
                EndTurnBackAction(_TurnBackTimeTick);
                _TurnBackTimeTick = 0;
            }

            if (currentTimeTick > _LastTimeTick)
            {
                _LastTimeTick = currentTimeTick;
                _CurrentSeqNumber = MinSeqNumber;

                return CalcId(_LastTimeTick);
            }

            if (_CurrentSeqNumber > MaxSeqNumber)
            {
                BeginOverCostAction(currentTimeTick);

                _TermIndex++;
                _LastTimeTick++;
                _CurrentSeqNumber = MinSeqNumber;
                _IsOverCost = true;
                _OverCostCountInOneTerm = 1;
                _GenCountInOneTerm = 1;

                return CalcId(_LastTimeTick);
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
                ((long)WorkerId << SeqBitLength) + _TurnBackIndex);

            _TurnBackTimeTick--;
            return result;
        }

        protected virtual long GetCurrentTimeTick()
        {
            //return (long)(DateTime.UtcNow - BaseTime).Ticks;
            //return (long)(_StartTimeTick + Environment.TickCount);
            return (long)(DateTime.UtcNow - BaseTime).TotalMilliseconds;
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
