using System;
using System.Collections.Generic;
using System.Text;

namespace Yitter.IdGenerator
{
    /// <summary>
    /// 常规雪花算法
    /// </summary>
    internal class SnowWorkerM2 : SnowWorkerM1
    {
        public SnowWorkerM2(IdGeneratorOptions options) : base(options)
        {

        }

        public override long NextId()
        {
            lock (_SyncLock)
            {
                long currentTimeTick = GetCurrentTimeTick();

                if (_LastTimeTick == currentTimeTick)
                {
                    if (_CurrentSeqNumber++ > MaxSeqNumber)
                    {
                        _CurrentSeqNumber = MinSeqNumber;
                        currentTimeTick = GetNextTimeTick();
                    }
                }
                else
                {
                    _CurrentSeqNumber = MinSeqNumber;
                }

                if (currentTimeTick < _LastTimeTick)
                {
                    throw new Exception(string.Format("Time error for {0} milliseconds", _LastTimeTick - currentTimeTick));
                }

                _LastTimeTick = currentTimeTick;
                var result = ((currentTimeTick << _TimestampShift) + ((long)WorkerId << SeqBitLength) + (uint)_CurrentSeqNumber);

                return result;
            }
        }

    }
}
