/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 * 版权协议：MIT
 * 版权说明：只要保留本版权，你可以免费使用、修改、分发本代码。
 * 免责条款：任何因为本代码产生的系统、法律、政治、宗教问题，均与版权所有者无关。
 * 
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Yitter.IdGenerator
{
    /// <summary>
    /// 默认实现
    /// </summary>
    public class DefaultIdGenerator : IIdGenerator
    {
        private ISnowWorker _SnowWorker { get; set; }

        public Action<OverCostActionArg> GenIdActionAsync
        {
            get => _SnowWorker.GenAction;
            set => _SnowWorker.GenAction = value;
        }


        public DefaultIdGenerator(IdGeneratorOptions options)
        {
            if (options == null)
            {
                throw new ApplicationException("options error.");
            }

            // 1.BaseTime
            if (options.BaseTime < DateTime.Now.AddYears(-50) || options.BaseTime > DateTime.Now)
            {
                throw new ApplicationException("BaseTime error.");
            }

            // 2.WorkerIdBitLength
            if (options.WorkerIdBitLength <= 0)
            {
                throw new ApplicationException("WorkerIdBitLength error.(range:[1, 21])");
            }
            if (options.DataCenterIdBitLength + options.WorkerIdBitLength + options.SeqBitLength > 22)
            {
                throw new ApplicationException("error：DataCenterIdBitLength + WorkerIdBitLength + SeqBitLength <= 22");
            }

            // 3.WorkerId & DataCenterId
            var maxWorkerIdNumber = (1 << options.WorkerIdBitLength) - 1;
            if (maxWorkerIdNumber == 0)
            {
                maxWorkerIdNumber = 63;
            }
            if (options.WorkerId < 0 || options.WorkerId > maxWorkerIdNumber)
            {
                throw new ApplicationException("WorkerId error. (range:[0, " + maxWorkerIdNumber + "]");
            }

            var maxDataCenterIdNumber = (1 << options.DataCenterIdBitLength) - 1;
            if (options.DataCenterId < 0 || options.DataCenterId > maxDataCenterIdNumber)
            {
                throw new ApplicationException("DataCenterId error. (range:[0, " + maxDataCenterIdNumber + "]");
            }

            // 4.SeqBitLength
            if (options.SeqBitLength < 2 || options.SeqBitLength > 21)
            {
                throw new ApplicationException("SeqBitLength error. (range:[2, 21])");
            }

            // 5.MaxSeqNumber
            var maxSeqNumber = (1 << options.SeqBitLength) - 1;
            if (maxSeqNumber == 0)
            {
                maxSeqNumber = 63;
            }
            if (options.MaxSeqNumber < 0 || options.MaxSeqNumber > maxSeqNumber)
            {
                throw new ApplicationException("MaxSeqNumber error. (range:[1, " + maxSeqNumber + "]");
            }

            // 6.MinSeqNumber
            if (options.MinSeqNumber < 5 || options.MinSeqNumber > maxSeqNumber)
            {
                throw new ApplicationException("MinSeqNumber error. (range:[5, " + maxSeqNumber + "]");
            }

            switch (options.Method)
            {
                case 2:
                    _SnowWorker = new SnowWorkerM2(options);
                    break;
                default:
                    if (options.DataCenterIdBitLength == 0 && options.TimestampType == 0)
                    {
                        _SnowWorker = new SnowWorkerM1(options);
                    }
                    else
                    {
                        _SnowWorker = new SnowWorkerM3(options);
                    }
                    break;
            }

            if (options.Method != 2)
            {
                Thread.Sleep(500);
            }
        }


        public long NewLong()
        {
            return _SnowWorker.NextId();
        }
    }
}
