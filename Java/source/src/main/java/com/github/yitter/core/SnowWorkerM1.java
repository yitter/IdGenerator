/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.github.yitter.core;

import com.github.yitter.contract.ISnowWorker;
import com.github.yitter.contract.IdGeneratorOptions;
import com.github.yitter.contract.OverCostActionArg;
import com.github.yitter.contract.IdGeneratorException;

public class SnowWorkerM1 implements ISnowWorker {

    /**
     * 基础时间
     */
    protected final long BaseTime;

    /**
     * 机器码
     */
    protected final short WorkerId;

    /**
     * 机器码位长
     */
    protected final byte WorkerIdBitLength;

    /**
     * 自增序列数位长
     */
    protected final byte SeqBitLength;

    /**
     * 最大序列数（含）
     */
    protected final int MaxSeqNumber;

    /**
     * 最小序列数（含）
     */
    protected final short MinSeqNumber;

    /**
     * 最大漂移次数
     */
    protected final int TopOverCostCount;

    protected final byte _TimestampShift;
    protected final static byte[] _SyncLock = new byte[0];

    protected short _CurrentSeqNumber;
    protected long _LastTimeTick = -1L;
    protected long _TurnBackTimeTick = -1L;
    protected byte _TurnBackIndex = 0;

    protected boolean _IsOverCost = false;
    protected int _OverCostCountInOneTerm = 0;
    protected int _GenCountInOneTerm = 0;
    protected int _TermIndex = 0;

    public SnowWorkerM1(IdGeneratorOptions options) {
        WorkerId = options.WorkerId;
        WorkerIdBitLength = options.WorkerIdBitLength == 0 ? 6 : options.WorkerIdBitLength;
        SeqBitLength = options.SeqBitLength == 0 ? 6 : options.SeqBitLength;
        MaxSeqNumber = options.MaxSeqNumber > 0 ? options.MaxSeqNumber : (int) Math.pow(2, SeqBitLength) - 1;
        MinSeqNumber = options.MinSeqNumber;
        TopOverCostCount = options.TopOverCostCount;
        BaseTime = options.BaseTime != 0 ? options.BaseTime : 1582136402000L;
        _TimestampShift = (byte) (WorkerIdBitLength + SeqBitLength);
        _CurrentSeqNumber = options.MinSeqNumber;
    }

    private void DoGenIdAction(OverCostActionArg arg) {

    }

    private void BeginOverCostAction(long useTimeTick) {
//        if (GenAction == null) {
//            return;
//        }
//
//        DoGenIdAction(new OverCostActionArg(
//                WorkerId,
//                useTimeTick,
//                1,
//                _OverCostCountInOneTerm,
//                _GenCountInOneTerm,
//                _TermIndex));
    }

    private void EndOverCostAction(long useTimeTick) {
        if (_TermIndex > 10000) {
            _TermIndex = 0;
        }
//
//        if (GenAction == null) {
//            return;
//        }
//
//        DoGenIdAction(new OverCostActionArg(
//                WorkerId,
//                useTimeTick,
//                2,
//                _OverCostCountInOneTerm,
//                _GenCountInOneTerm,
//                _TermIndex));
    }

    private void BeginTurnBackAction(long useTimeTick) {
//        if (GenAction == null) {
//            return;
//        }
//
//        DoGenIdAction(new OverCostActionArg(
//                WorkerId,
//                useTimeTick,
//                8,
//                _OverCostCountInOneTerm,
//                _GenCountInOneTerm,
//                _TermIndex));
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

    private long NextNormalId() throws IdGeneratorException {
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

            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
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
                ((long) WorkerId << SeqBitLength) +
                (int) _CurrentSeqNumber);

        _CurrentSeqNumber++;
        return result;
    }

    private long CalcTurnBackId(long useTimeTick) {
        long result = ((useTimeTick << _TimestampShift) +
                ((long) WorkerId << SeqBitLength) + _TurnBackIndex);

        _TurnBackTimeTick--;
        return result;
    }

    protected long GetCurrentTimeTick() {
        long millis = System.currentTimeMillis();
        return millis - BaseTime;
    }

    protected long GetNextTimeTick() {
        long tempTimeTicker = GetCurrentTimeTick();

        while (tempTimeTicker <= _LastTimeTick) {
            tempTimeTicker = GetCurrentTimeTick();
        }

        return tempTimeTicker;
    }

    @Override
    public long nextId() {
        synchronized (_SyncLock) {
            return _IsOverCost ? NextOverCostId() : NextNormalId();
        }
    }
}

