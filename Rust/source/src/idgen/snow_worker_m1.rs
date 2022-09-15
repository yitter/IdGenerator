/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 */
use crate::idgen::*;
use chrono::Utc;
use std::thread;
use std::thread::sleep;
// use lazy_static::lazy_static;

pub struct SnowWorkerM1 {
    ///基础时间
    pub BaseTime: i64,
    ///机器码
    pub WorkerId: u32,
    ///机器码位长
    pub WorkerIdBitLength: u8,
    ///自增序列数位长
    pub SeqBitLength: u8,
    ///最大序列数（含）
    pub MaxSeqNumber: u32,
    ///最小序列数（含）
    pub MinSeqNumber: u32,
    ///最大漂移次数
    pub TopOverCostCount: u32,

    _TimestampShift: u8,
    _CurrentSeqNumber: u32,
    _LastTimeTick: i64,
    _TurnBackTimeTick: i64,
    _TurnBackIndex: u8,
    _IsOverCost: bool,
    _OverCostCountInOneTerm: u32,
    _GenCountInOneTerm: u32,
    _TermIndex: u32,
}

impl SnowWorkerM1 {
    pub fn Default() -> SnowWorkerM1 {
        let options = IdGeneratorOptions::New(1);
        return SnowWorkerM1::New(options);
    }

    pub fn SetOptions(&mut self, options: IdGeneratorOptions) {
        // 1.BaseTime
        if options.BaseTime == 0 {
            self.BaseTime = 1582136402000;
        } else if options.BaseTime < 631123200000
            || options.BaseTime > Utc::now().timestamp_millis()
        {
            panic!("BaseTime error.");
        } else {
            self.BaseTime = options.BaseTime;
        }

        // 2.WorkerIdBitLength
        if options.WorkerIdBitLength <= 0 {
            panic!("WorkerIdBitLength error.(range:[1, 21])");
        }
        if options.SeqBitLength + options.WorkerIdBitLength > 22 {
            panic!("error：WorkerIdBitLength + SeqBitLength <= 22");
        } else {
            // self.WorkerIdBitLength = options.WorkerIdBitLength;
            self.WorkerIdBitLength = if options.WorkerIdBitLength <= 0 {
                6
            } else {
                options.WorkerIdBitLength
            };
        }

        // 3.WorkerId
        let mut maxWorkerIdNumber = (1 << options.WorkerIdBitLength) - 1;
        if maxWorkerIdNumber == 0 {
            maxWorkerIdNumber = 63;
        }
        if options.WorkerId < 0 || options.WorkerId > maxWorkerIdNumber {
            panic!("WorkerId error. (range:[0, {} ]", maxWorkerIdNumber);
        } else {
            self.WorkerId = options.WorkerId;
        }

        // 4.SeqBitLength
        if options.SeqBitLength < 2 || options.SeqBitLength > 21 {
            panic!("SeqBitLength error. (range:[2, 21])");
        } else {
            // self.SeqBitLength = options.SeqBitLength;
            self.SeqBitLength = if options.SeqBitLength <= 0 {
                6
            } else {
                options.SeqBitLength
            };
        }

        // 5.MaxSeqNumber
        let mut maxSeqNumber = (1 << options.SeqBitLength) - 1;
        if maxSeqNumber == 0 {
            maxSeqNumber = 63;
        }
        if options.MaxSeqNumber < 0 || options.MaxSeqNumber > maxSeqNumber {
            panic!("MaxSeqNumber error. (range:[1, {}]", maxSeqNumber);
        } else {
            self.MaxSeqNumber = if options.MaxSeqNumber == 0 {
                maxSeqNumber
            } else {
                options.MaxSeqNumber
            };
        }

        // 6.MinSeqNumber
        if options.MinSeqNumber < 5 || options.MinSeqNumber > maxSeqNumber {
            panic!("MinSeqNumber error. (range:[5, {}]", maxSeqNumber);
        } else {
            self.MinSeqNumber = options.MinSeqNumber;
            // self.MinSeqNumber = if options.MinSeqNumber <= 0 { 5 } else { options.MinSeqNumber };
        }

        // 7.TopOverCostCount
        //self.TopOverCostCount = if options.TopOverCostCount == 0 { 2000 } else { options.TopOverCostCount };
        if options.TopOverCostCount < 0 || options.TopOverCostCount > 10000 {
            panic!("TopOverCostCount error. (range:[0, 10000]");
        } else {
            self.TopOverCostCount = options.TopOverCostCount;
        }

        // 8.Others
        self._TimestampShift = self.WorkerIdBitLength + self.SeqBitLength;
        self._CurrentSeqNumber = self.MinSeqNumber;

        if options.Method == 1 {
            sleep(std::time::Duration::from_millis(500));
        }
    }

    pub fn New(options: IdGeneratorOptions) -> SnowWorkerM1 {
        let mut worker = SnowWorkerM1 {
            BaseTime: 1582136402000,
            WorkerIdBitLength: 0,
            WorkerId: 0,
            SeqBitLength: 0,
            MaxSeqNumber: 0,
            MinSeqNumber: 0,
            TopOverCostCount: 0,
            _TimestampShift: 0,
            _CurrentSeqNumber: 0,

            _LastTimeTick: 0,
            _TurnBackTimeTick: 0,
            _TurnBackIndex: 0,
            _IsOverCost: false,
            _OverCostCountInOneTerm: 0,
            _GenCountInOneTerm: 0,
            _TermIndex: 0,
        };

        worker.SetOptions(options);
        return worker;
    }

    pub fn NextId(&mut self) -> i64 {
        // println!("SeqBitLength: {}", self.SeqBitLength);
        if self._IsOverCost {
            self.NextOverCostId()
        } else {
            self.NextNormalId()
        }
    }

    fn DoGenIdAction(&self, arg: OverCostActionArg) {}

    fn BeginOverCostAction(&self, useTimeTick: i64) {}

    fn EndOverCostAction(&mut self, useTimeTick: i64) {
        if self._TermIndex > 10000 {
            self._TermIndex = 0;
        }
    }

    fn BeginTurnBackAction(&self, useTimeTick: i64) {}

    fn EndTurnBackAction(&self, useTimeTick: i64) {}

    fn NextOverCostId(&mut self) -> i64 {
        let currentTimeTick = self.GetCurrentTimeTick();

        if currentTimeTick > self._LastTimeTick {
            self.EndOverCostAction(currentTimeTick);

            self._LastTimeTick = currentTimeTick;
            self._CurrentSeqNumber = self.MinSeqNumber;
            self._IsOverCost = false;
            self._OverCostCountInOneTerm = 0;
            self._GenCountInOneTerm = 0;

            return self.CalcId(self._LastTimeTick);
        }

        if self._OverCostCountInOneTerm >= self.TopOverCostCount {
            self.EndOverCostAction(currentTimeTick);

            self._LastTimeTick = self.GetNextTimeTick();
            self._CurrentSeqNumber = self.MinSeqNumber;
            self._IsOverCost = false;
            self._OverCostCountInOneTerm = 0;
            self._GenCountInOneTerm = 0;

            return self.CalcId(self._LastTimeTick);
        }

        if self._CurrentSeqNumber > self.MaxSeqNumber {
            self._LastTimeTick += 1;
            self._CurrentSeqNumber = self.MinSeqNumber;
            self._IsOverCost = true;
            self._OverCostCountInOneTerm += 1;
            self._GenCountInOneTerm += 1;

            return self.CalcId(self._LastTimeTick);
        }

        self._GenCountInOneTerm += 1;
        return self.CalcId(self._LastTimeTick);
    }

    fn NextNormalId(&mut self) -> i64 {
        let currentTimeTick = self.GetCurrentTimeTick();

        if currentTimeTick < self._LastTimeTick {
            if self._TurnBackTimeTick < 1 {
                self._TurnBackTimeTick = self._LastTimeTick - 1;
                self.BeginTurnBackAction(self._TurnBackTimeTick);
            }

            self._TurnBackIndex += 1;
            // 每毫秒序列数的前5位是预留位，0用于手工新值，1-4是时间回拨次序
            // 支持4次回拨次序（避免回拨重叠导致ID重复），可无限次回拨（次序循环使用）。
            if self._TurnBackIndex > 4 {
                self._TurnBackIndex = 1;
            }

            // thread::sleep(std::time::Duration::from_millis(1));
            return self.CalcTurnBackId(self._TurnBackTimeTick);
        }

        // 时间追平时，_TurnBackTimeTick清零
        if self._TurnBackTimeTick > 0 {
            self.EndTurnBackAction(self._TurnBackTimeTick);
            self._TurnBackTimeTick = 0;
        }

        if currentTimeTick > self._LastTimeTick {
            self._LastTimeTick = currentTimeTick;
            self._CurrentSeqNumber = self.MinSeqNumber;

            return self.CalcId(self._LastTimeTick);
        }

        if self._CurrentSeqNumber > self.MaxSeqNumber {
            self.BeginOverCostAction(currentTimeTick);

            self._TermIndex += 1;
            self._LastTimeTick += 1;
            self._CurrentSeqNumber = self.MinSeqNumber;
            self._IsOverCost = true;
            self._OverCostCountInOneTerm = 1;
            self._GenCountInOneTerm = 1;

            return self.CalcId(self._LastTimeTick);
        }

        return self.CalcId(self._LastTimeTick);
    }

    fn CalcId(&mut self, useTimeTick: i64) -> i64 {
        let result = (useTimeTick << self._TimestampShift)
            + (self.WorkerId << self.SeqBitLength) as i64
            + (self._CurrentSeqNumber) as i64;
        self._CurrentSeqNumber += 1;
        return result;
    }

    fn CalcTurnBackId(&mut self, useTimeTick: i64) -> i64 {
        let result = (useTimeTick << self._TimestampShift)
            + (self.WorkerId << self.SeqBitLength) as i64
            + (self._TurnBackIndex) as i64;
        self._TurnBackTimeTick -= 1;
        return result;
    }

    fn GetCurrentTimeTick(&self) -> i64 {
        return Utc::now().timestamp_millis() - self.BaseTime;
    }

    fn GetNextTimeTick(&self) -> i64 {
        let mut tempTimeTicker = self.GetCurrentTimeTick();

        while tempTimeTicker <= self._LastTimeTick {
            // 暂停1ms
            sleep(std::time::Duration::from_millis(1));
            tempTimeTicker = self.GetCurrentTimeTick();
        }

        return tempTimeTicker;
    }
}
