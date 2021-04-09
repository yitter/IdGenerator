/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.idgen.DefaultIdGenerator;

import yitter.contract.IIdGenerator;
import yitter.contract.ISnowWorker;
import yitter.contract.IdGeneratorException;
import yitter.contract.IdGeneratorOptions;

import yitter.core.DateTimeHelper;
import yitter.core.SnowWorkerM1;
import yitter.core.SnowWorkerM2;

import core.thread;
import core.time;

import std.conv;
import std.datetime;
import std.stdio;

/**
 * 
 */
class DefaultIdGenerator : IIdGenerator {

    private __gshared ISnowWorker _SnowWorker = null;

    this(IdGeneratorOptions options) {
        if (options is null) {
            throw new IdGeneratorException("options error.");
        }

        // 1.BaseTime
        if (options.BaseTime < 315504000000L || options.BaseTime > DateTimeHelper.currentTimeMillis) {
            throw new IdGeneratorException("BaseTime error.");
        }

        // 2.WorkerIdBitLength
        if (options.WorkerIdBitLength <= 0) {
            throw new IdGeneratorException("WorkerIdBitLength error.(range:[1, 21])");
        }
        if (options.WorkerIdBitLength + options.SeqBitLength > 22) {
            throw new IdGeneratorException("error：WorkerIdBitLength + SeqBitLength <= 22");
        }

        // 3.WorkerId
        int maxWorkerIdNumber = (1 << options.WorkerIdBitLength) - 1;
        if (maxWorkerIdNumber == 0) {
            maxWorkerIdNumber = 63;
        }
        if (options.WorkerId < 0 || options.WorkerId > maxWorkerIdNumber) {
            string msg = "WorkerId error. (range:[0, " ~ to!string((maxWorkerIdNumber > 0 ? maxWorkerIdNumber : 63)) ~ "]";
            throw new IdGeneratorException(msg);
        }

        // 4.SeqBitLength
        if (options.SeqBitLength < 2 || options.SeqBitLength > 21) {
            throw new IdGeneratorException("SeqBitLength error. (range:[2, 21])");
        }

        // 5.MaxSeqNumber
        int maxSeqNumber = (1 << options.SeqBitLength) - 1;
        if (maxSeqNumber == 0) {
            maxSeqNumber = 63;
        }
        if (options.MaxSeqNumber < 0 || options.MaxSeqNumber > maxSeqNumber) {
            throw new IdGeneratorException("MaxSeqNumber error. (range:[1, " ~ maxSeqNumber.to!string() ~ "]");
        }

        // 6.MinSeqNumber
        if (options.MinSeqNumber < 5 || options.MinSeqNumber > maxSeqNumber) {
            throw new IdGeneratorException("MinSeqNumber error. (range:[5, " ~ maxSeqNumber.to!string() ~ "]");
        }

        switch (options.Method) {
            case 2:
                _SnowWorker = new SnowWorkerM2(options);
                break;
            case 1:
            default:
                _SnowWorker = new SnowWorkerM1(options);
                break;
        }

        if (options.Method == 1) {
            try {
                Thread.sleep(500.msecs);
            } catch (Exception e) {
                writeln(e.toString());
            }
        }
    }

    long newLong() {
        return _SnowWorker.nextId();
    }
}
