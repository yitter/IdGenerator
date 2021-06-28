/**
 * @description: 
 * @author: bubao
 */
class Genid {
    /**
     *Creates an instance of Genid.
     * @author bubao
     * @param {{
     *     Method: 1, // 雪花计算方法，（1-漂移算法|2-传统算法），默认 1
     *     BaseTime: 1577836800000,  // 基础时间（ms 单位），不能超过当前系统时间
     *     WorkerId: Number, // 机器码，必须由外部设定，最大值 2^WorkerIdBitLength-1
     *     WorkerIdBitLength: 6,   // 机器码位长，默认值 6，取值范围 [1, 15](要求：序列数位长+机器码位长不超过 22)
     *     SeqBitLength: 6,   // 序列数位长，默认值 6，取值范围 [3, 21](要求：序列数位长+机器码位长不超过 22)
     *     MaxSeqNumber: 5, // 最大序列数（含），设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值 0，表示最大序列数取最大值（2^SeqBitLength-1]）
     *     MinSeqNumber: 5, // 最小序列数（含），默认值 5，取值范围 [5, MaxSeqNumber]，每毫秒的前 5 个序列数对应编号 0-4 是保留位，其中 1-4 是时间回拨相应预留位，0 是手工新值预留位
     *     TopOverCostCount: 2000// 最大漂移次数（含），默认 2000，推荐范围 500-10000（与计算能力有关）
     * }} options
     * @memberof Genid
     */
    constructor(options) {
        if (options.WorkerId === undefined) {
            throw new Error("lost WorkerId");
        }
        // 1.BaseTime
        const BaseTime = 1577836800000;
        if (!options.BaseTime || options.BaseTime < 0) {
            options.BaseTime = BaseTime;
        }
        // 2.WorkerIdBitLength
        const WorkerIdBitLength = 6;
        if (!options.WorkerIdBitLength || options.WorkerIdBitLength < 0) {
            options.WorkerIdBitLength = WorkerIdBitLength;
        }

        // 4.SeqBitLength
        const SeqBitLength = 6;
        if (!options.SeqBitLength || options.SeqBitLength < 0) {
            options.SeqBitLength = SeqBitLength;
        }
        // 5.MaxSeqNumber
        const MaxSeqNumber = (1 << SeqBitLength) - 1;
        if (options.MaxSeqNumber <= 0 || options.MaxSeqNumber === undefined) {
            options.MaxSeqNumber = MaxSeqNumber;
        }
        // 6.MinSeqNumber
        const MinSeqNumber = 5;
        if (!options.MinSeqNumber || options.MinSeqNumber < 0) {
            options.MinSeqNumber = MinSeqNumber;
        }
        // 7.Others
        const topOverCostCount = 2000;
        if (!options.TopOverCostCount || options.TopOverCostCount < 0) {
            options.TopOverCostCount = topOverCostCount;
        }

        if (options.Method !== 2) {
            options.Method = 1;
        } else {
            options.Method = 2;
        }

        this.Method = BigInt(options.Method);
        this.BaseTime = BigInt(options.BaseTime);
        this.WorkerId = BigInt(options.WorkerId);
        this.WorkerIdBitLength = BigInt(options.WorkerIdBitLength);
        this.SeqBitLength = BigInt(options.SeqBitLength);
        this.MaxSeqNumber = BigInt(options.MaxSeqNumber);
        this.MinSeqNumber = BigInt(options.MinSeqNumber);
        this.TopOverCostCount = BigInt(options.TopOverCostCount);

        const timestampShift = this.WorkerIdBitLength + this.SeqBitLength;
        const currentSeqNumber = this.MinSeqNumber;

        this._TimestampShift = timestampShift;
        this._CurrentSeqNumber = currentSeqNumber;

        this._LastTimeTick = 0;
        this._TurnBackTimeTick = 0;
        this._TurnBackIndex = 0;
        this._IsOverCost = false;
        this._OverCostCountInOneTerm = 0;
    }

    // DoGenIDAction .
    DoGenIdAction(OverCostActionArg) { }

    BeginOverCostAction(useTimeTick) { }

    EndOverCostAction(useTimeTick) {
        // if m1._TermIndex > 10000 {
        //     m1._TermIndex = 0
        // }
    }

    BeginTurnBackAction(useTimeTick) { }

    EndTurnBackAction(useTimeTick) { }

    NextOverCostId() {
        const currentTimeTick = this.GetCurrentTimeTick();
        if (currentTimeTick > this._LastTimeTick) {
            // this.EndOverCostAction(currentTimeTick)
            this._LastTimeTick = currentTimeTick;
            this._CurrentSeqNumber = this.MinSeqNumber;
            this._IsOverCost = false;
            this._OverCostCountInOneTerm = 0;
            // this._GenCountInOneTerm = 0
            return this.CalcId(this._LastTimeTick);
        }
        if (this._OverCostCountInOneTerm >= this.TopOverCostCount) {
            // this.EndOverCostAction(currentTimeTick)
            this._LastTimeTick = this.GetNextTimeTick();
            this._CurrentSeqNumber = this.MinSeqNumber;
            this._IsOverCost = false;
            this._OverCostCountInOneTerm = 0;
            // this._GenCountInOneTerm = 0
            return this.CalcId(this._LastTimeTick);
        }
        if (this._CurrentSeqNumber > this.MaxSeqNumber) {
            this._LastTimeTick++;
            this._CurrentSeqNumber = this.MinSeqNumber;
            this._IsOverCost = true;
            this._OverCostCountInOneTerm++;
            // this._GenCountInOneTerm++

            return this.CalcId(this._LastTimeTick);
        }

        // this._GenCountInOneTerm++
        return this.CalcId(this._LastTimeTick);
    }

    NextNormalId() {
        const currentTimeTick = this.GetCurrentTimeTick();
        if (currentTimeTick < this._LastTimeTick) {
            if (this._TurnBackTimeTick < 1) {
                this._TurnBackTimeTick = this._LastTimeTick - 1;
                this._TurnBackIndex++;
                // 每毫秒序列数的前 5 位是预留位，0 用于手工新值，1-4 是时间回拨次序
                // 支持 4 次回拨次序（避免回拨重叠导致 ID 重复），可无限次回拨（次序循环使用）。
                if (this._TurnBackIndex > 4) {
                    this._TurnBackIndex = 1;
                }
                this.BeginTurnBackAction(this._TurnBackTimeTick);
            }
            return this.CalcTurnBackId(this._TurnBackTimeTick);
        }
        // 时间追平时，_TurnBackTimeTick 清零
        if (this._TurnBackTimeTick > 0) {
            this.EndTurnBackAction(this._TurnBackTimeTick);
            this._TurnBackTimeTick = 0;
        }

        if (currentTimeTick > this._LastTimeTick) {
            this._LastTimeTick = currentTimeTick;
            this._CurrentSeqNumber = this.MinSeqNumber;
            return this.CalcId(this._LastTimeTick);
        }

        if (this._CurrentSeqNumber > this.MaxSeqNumber) {
            this.BeginOverCostAction(currentTimeTick);
            // this._TermIndex++
            this._LastTimeTick++;
            this._CurrentSeqNumber = this.MinSeqNumber;
            this._IsOverCost = true;
            this._OverCostCountInOneTerm = 1;
            // this._GenCountInOneTerm = 1

            return this.CalcId(this._LastTimeTick);
        }

        return this.CalcId(this._LastTimeTick);
    }

    CalcId(useTimeTick) {
        const result = BigInt(useTimeTick << this._TimestampShift) + BigInt(this.WorkerId << this.SeqBitLength) + BigInt(this._CurrentSeqNumber);
        this._CurrentSeqNumber++;
        return result;
    }

    CalcTurnBackId(useTimeTick) {
        const result = BigInt(useTimeTick << this._TimestampShift) + BigInt(this.WorkerId << this.SeqBitLength) + BigInt(this._TurnBackIndex);
        this._TurnBackTimeTick--;
        return result;
    }

    GetCurrentTimeTick() {
        const millis = BigInt((new Date()).valueOf());
        return millis - this.BaseTime;
    }

    GetNextTimeTick() {
        let tempTimeTicker = this.GetCurrentTimeTick();
        while (tempTimeTicker <= this._LastTimeTick) {
            tempTimeTicker = this.GetCurrentTimeTick();
        }
        return tempTimeTicker;
    }

    NextId() {
        if (this._IsOverCost) {
            return parseInt(this.NextOverCostId());
        } else {
            return parseInt(this.NextNormalId());
        }
    }
}

module.exports = Genid;