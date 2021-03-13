/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.yitter.core;

import com.yitter.contract.ISnowWorker;
import com.yitter.contract.IdGeneratorException;
import com.yitter.contract.IdGeneratorOptions;
import com.yitter.contract.OverCostActionArg;

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
     * （机器码+序列数<=22位）
     */
    protected final byte WorkerIdBitLength;

    /**
     * 自增序列数位长
     * （机器码+序列数<=22位）
     */
    protected final byte SeqBitLength;

    /**
     * 最大序列数（含此值）
     * 超过最大值，就会从MinSeqNumber开始
     */
    protected final int MaxSeqNumber;

    /**
     * 最小序列数（含此值）
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
    protected boolean _IsOverCost = false;
    protected int _OverCostCountInOneTerm = 0;
    protected int _GenCountInOneTerm = 0;
    protected int _TermIndex = 0;

    public SnowWorkerM1(IdGeneratorOptions options) {
        WorkerId = options.WorkerId;
        WorkerIdBitLength = options.WorkerIdBitLength == 0 ? 6 : options.WorkerIdBitLength;
        SeqBitLength = options.SeqBitLength == 0 ? 6 : options.SeqBitLength;
        MaxSeqNumber = options.MaxSeqNumber > 0 ? options.MaxSeqNumber : (int) Math.pow(2, SeqBitLength);
        MinSeqNumber = options.MinSeqNumber;
        TopOverCostCount = options.TopOverCostCount;
        BaseTime = options.BaseTime != 0 ? options.BaseTime : 1582136402000L;
        _TimestampShift = (byte) (WorkerIdBitLength + SeqBitLength);
        _CurrentSeqNumber = options.MinSeqNumber;
    }

    private void DoGenIdAction(OverCostActionArg arg) {

    }

    private void BeginOverCostCallBack(long useTimeTick) {
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

    private void EndOverCostCallBack(long useTimeTick) {
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

    private void TurnBackCallBack(long useTimeTick) {
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

    private long NextOverCostId() {
        long currentTimeTick = GetCurrentTimeTick();

        if (currentTimeTick > _LastTimeTick) {
            EndOverCostCallBack(currentTimeTick);

            _LastTimeTick = currentTimeTick;
            _CurrentSeqNumber = MinSeqNumber;
            _IsOverCost = false;
            _OverCostCountInOneTerm = 0;
            _GenCountInOneTerm = 0;

            return CalcId(_LastTimeTick);
        }

        if (_OverCostCountInOneTerm >= TopOverCostCount) {
            EndOverCostCallBack(currentTimeTick);

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

        if (currentTimeTick > _LastTimeTick) {
            _LastTimeTick = currentTimeTick;
            _CurrentSeqNumber = MinSeqNumber;

            return CalcId(_LastTimeTick);
        }

        if (_CurrentSeqNumber > MaxSeqNumber) {
            BeginOverCostCallBack(currentTimeTick);

            _TermIndex++;
            _LastTimeTick++;
            _CurrentSeqNumber = MinSeqNumber;
            _IsOverCost = true;
            _OverCostCountInOneTerm++;
            _GenCountInOneTerm = 1;

            return CalcId(_LastTimeTick);
        }

        if (currentTimeTick < _LastTimeTick) {
            if (_TurnBackTimeTick < 1) {
                _TurnBackTimeTick = _LastTimeTick - 1;
            }

            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            TurnBackCallBack(_TurnBackTimeTick);

            return CalcTurnBackId(_TurnBackTimeTick);
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
                ((long) WorkerId << SeqBitLength) + 0);

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

