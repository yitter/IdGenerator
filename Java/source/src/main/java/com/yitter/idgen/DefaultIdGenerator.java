/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.yitter.idgen;

import com.yitter.contract.ISnowWorker;
import com.yitter.contract.IdGeneratorException;
import com.yitter.contract.IdGeneratorOptions;
import com.yitter.contract.IIdGenerator;
import com.yitter.core.SnowWorkerM1;
import com.yitter.core.SnowWorkerM2;


public class DefaultIdGenerator implements IIdGenerator {

    private final ISnowWorker _SnowWorker;

    public DefaultIdGenerator(IdGeneratorOptions options) throws IdGeneratorException {
        if (options == null)
        {
            throw new IdGeneratorException("options error.");
        }

        if (options.StartTime > System.currentTimeMillis())
        {
            throw new IdGeneratorException("StartTime error.");
        }

        if (options.SeqBitLength + options.WorkerIdBitLength > 22)
        {
            throw new IdGeneratorException("error：WorkerIdBitLength + SeqBitLength <= 22");
        }

        double maxWorkerIdNumber = Math.pow(2, options.WorkerIdBitLength) - 1;
        if (options.WorkerId < 1 || options.WorkerId > maxWorkerIdNumber)
        {
            throw new IdGeneratorException("WorkerId error. (range:[1, " + maxWorkerIdNumber + "]");
        }

        if (options.SeqBitLength < 2 || options.SeqBitLength > 21)
        {
            throw new IdGeneratorException("SeqBitLength error. (range:[2, 21])");
        }

        double maxSeqNumber = Math.pow(2, options.SeqBitLength) - 1;
        if (options.MaxSeqNumber < 0 || options.MaxSeqNumber > maxSeqNumber)
        {
            throw new IdGeneratorException("MaxSeqNumber error. (range:[1, " + maxSeqNumber + "]");
        }

        double maxValue = maxSeqNumber - 2;
        if (options.MinSeqNumber < 5 || options.MinSeqNumber > maxValue)
        {
            throw new IdGeneratorException("MinSeqNumber error. (range:[5, " + maxValue + "]");
        }

        switch (options.Method)
        {
            case 1:
                _SnowWorker = new SnowWorkerM1(options);
                break;
            case 2:
                _SnowWorker = new SnowWorkerM2(options);
                break;
            default:
                _SnowWorker = new SnowWorkerM1(options);
                break;
        }

        if (options.Method == 1)
        {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public long newLong()  {
        return _SnowWorker.nextId();
    }
}
