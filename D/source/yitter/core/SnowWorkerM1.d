/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.core.SnowWorkerM1;

import yitter.contract.ISnowWorker;
import yitter.contract.IdGeneratorOptions;
import yitter.contract.OverCostActionArg;
import yitter.contract.IdGeneratorException;

import std.datetime;

class SnowWorkerM1 : ISnowWorker {

    /**
     * 基础时间
     */
    protected SysTime BaseTime;

    /**
     * 机器码
     */
    protected short WorkerId;

    /**
     * 机器码位长
     */
    protected byte WorkerIdBitLength;

    /**
     * 自增序列数位长
     */
    protected byte SeqBitLength;

    /**
     * 最大序列数（含）
     */
    protected int MaxSeqNumber;

    /**
     * 最小序列数（含）
     */
    protected short MinSeqNumber;

    /**
     * 最大漂移次数
     */
    protected int TopOverCostCount;

    protected byte _TimestampShift;
    // protected __gshared Object _SyncLock; // = new byte[0];

    protected short _CurrentSeqNumber;
    protected long _LastTimeTick = 0;
    protected long _TurnBackTimeTick = 0;
    protected byte _TurnBackIndex = 0;

    protected bool _IsOverCost = false;
    protected int _OverCostCountInOneTerm = 0;
    protected int _GenCountInOneTerm = 0;
    protected int _TermIndex = 0;

    // shared static this() {
    //     _SyncLock = new Object();
    // }

    this(IdGeneratorOptions options) {        
        BaseTime = options.BaseTime != SysTime.min ? options.BaseTime : SysTime(DateTime(2020, 2, 20, 2, 20, 2));
        WorkerIdBitLength = options.WorkerIdBitLength == 0 ? 6 : options.WorkerIdBitLength;
        WorkerId = options.WorkerId;
        SeqBitLength = options.SeqBitLength == 0 ? 6 : options.SeqBitLength;
        MaxSeqNumber = options.MaxSeqNumber <= 0 ? (1 << SeqBitLength) - 1 : options.MaxSeqNumber;
        MinSeqNumber = options.MinSeqNumber;
        TopOverCostCount = options.TopOverCostCount == 0 ? 2000 : options.TopOverCostCount;
        _TimestampShift = cast(byte) (WorkerIdBitLength + SeqBitLength);
        _CurrentSeqNumber = MinSeqNumber;
    }

    private void DoGenIdAction(OverCostActionArg arg) {

    }

    private void BeginOverCostAction(long useTimeTick) {

    }

    private void EndOverCostAction(long useTimeTick) {
        if (_TermIndex > 10000) {
            _TermIndex = 0;
        }
    }

    private void BeginTurnBackAction(long useTimeTick) {

    }

    private void EndTurnBackAction(long useTimeTick) {

    }

    private long NextOverCostId() {
        long currentTimeTick = GetCurrentTimeTick();

        if (currentTimeTick > _LastTimeTick) {
            EndOverCostAction(currentTimeTick);

            _LastTimeTick = currentTimeTick;
            _CurrentSeqNumber = MinSeqNumber;
            _IsOverCost = false;
            _OverCostCountInOneTerm = 0;
            _GenCountInOneTerm = 0;

            return CalcId(_LastTimeTick);
        }

        if (_OverCostCountInOneTerm >= TopOverCostCount) {
            EndOverCostAction(currentTimeTick);

            _LastTimeTick = GetNextTimeTick();
            _CurrentSeqNumber = MinSeqNumber;
            _IsOverCost = false;
            _OverCostCountInOneTerm = 0;
            _GenCountInOneTerm = 0;

            return CalcId(_LastTimeTick);
        }

        if (_CurrentSeqNumber > MaxSeqNumber) {
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

    private long NextNormalId() {
        long currentTimeTick = GetCurrentTimeTick();

        if (currentTimeTick < _LastTimeTick) {
            if (_TurnBackTimeTick < 1) {
                _TurnBackTimeTick = _LastTimeTick - 1;
                _TurnBackIndex++;

                // 每毫秒序列数的前5位是预留位，0用于手工新值，1-4是时间回拨次序
                // 最多4次回拨（防止回拨重叠）
                if (_TurnBackIndex > 4) {
                    _TurnBackIndex = 1;
                }
                BeginTurnBackAction(_TurnBackTimeTick);
            }

//            try {
//                 Thread.sleep(1);
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }

            return CalcTurnBackId(_TurnBackTimeTick);
        }

        // 时间追平时，_TurnBackTimeTick清零
        if (_TurnBackTimeTick > 0) {
            EndTurnBackAction(_TurnBackTimeTick);
            _TurnBackTimeTick = 0;
        }

        if (currentTimeTick > _LastTimeTick) {
            _LastTimeTick = currentTimeTick;
            _CurrentSeqNumber = MinSeqNumber;

            return CalcId(_LastTimeTick);
        }

        if (_CurrentSeqNumber > MaxSeqNumber) {
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

    private long CalcId(long useTimeTick) {
        long result = ((useTimeTick << _TimestampShift) +
                (cast(long) WorkerId << SeqBitLength) +
                cast(int) _CurrentSeqNumber);

        _CurrentSeqNumber++;
        return result;
    }

    private long CalcTurnBackId(long useTimeTick) {
        long result = ((useTimeTick << _TimestampShift) +
                (cast(long) WorkerId << SeqBitLength) + _TurnBackIndex);

        _TurnBackTimeTick--;
        return result;
    }

    protected long GetCurrentTimeTick() {
        SysTime now = Clock.currTime;
        Duration dur = Clock.currTime - BaseTime;
        return dur.total!("msecs");
    }

    protected long GetNextTimeTick() {
        long tempTimeTicker = GetCurrentTimeTick();

        while (tempTimeTicker <= _LastTimeTick) {
            tempTimeTicker = GetCurrentTimeTick();
        }

        return tempTimeTicker;
    }

    override
    long nextId() {
        synchronized {
            return _IsOverCost ? NextOverCostId() : NextNormalId();
        }
    }
}

