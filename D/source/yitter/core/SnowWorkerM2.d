/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.core.SnowWorkerM2;

import yitter.core.SnowWorkerM1;

import yitter.contract.IdGeneratorOptions;
import yitter.contract.IdGeneratorException;

import std.format;


/** 
 * 
 */
class SnowWorkerM2 : SnowWorkerM1 {

    this(IdGeneratorOptions options) {
        super(options);
    }

    override long nextId() {
        synchronized {
            long currentTimeTick = GetCurrentTimeTick();

            if (_LastTimeTick == currentTimeTick) {
                if (_CurrentSeqNumber++ > MaxSeqNumber) {
                    _CurrentSeqNumber = MinSeqNumber;
                    currentTimeTick = GetNextTimeTick();
                }
            } else {
                _CurrentSeqNumber = MinSeqNumber;
            }

            if (currentTimeTick < _LastTimeTick) {
                string msg = format("Time error for %d milliseconds", _LastTimeTick - currentTimeTick);
                throw new IdGeneratorException(msg);
            }

            _LastTimeTick = currentTimeTick;
            long result = ((currentTimeTick << _TimestampShift) + (cast(long) WorkerId << SeqBitLength) + cast(int) _CurrentSeqNumber);

            return result;
        }

    }
}
