using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Yitter.IdGenerator
{
    public class YitIdGenerator : IIdGenerator
    {
        private ISnowWorker _SnowWorker { get; set; }

        public Action<OverCostActionArg> GenIdActionAsync
        {
            get => _SnowWorker.GenAction;
            set => _SnowWorker.GenAction = value;
        }

        public YitIdGenerator(IdGeneratorOptions options)
        {
            if (options == null)
            {
                throw new ApplicationException("options error.");
            }

            if (options.StartTime > DateTime.Now)
            {
                throw new ApplicationException("StartTime error.");
            }

            if (options.SeqBitLength + options.WorkerIdBitLength > 22)
            {
                throw new ApplicationException("error：WorkerIdBitLength + SeqBitLength <= 22");
            }

            var maxWorkerIdNumber = Math.Pow(2, options.WorkerIdBitLength) - 1;
            if (options.WorkerId < 1 || options.WorkerId > maxWorkerIdNumber)
            {
                throw new ApplicationException("WorkerId is error. (range:[1, " + maxWorkerIdNumber + "]");
            }

            if (options.SeqBitLength < 2 || options.SeqBitLength > 21)
            {
                throw new ApplicationException("SeqBitLength is error. (range:[2, 21])");
            }

            var maxSeqNumber = Math.Pow(2, options.SeqBitLength) - 1;
            if (options.MaxSeqNumber < 0 || options.MaxSeqNumber > maxSeqNumber)
            {
                throw new ApplicationException("MaxSeqNumber is error. (range:[1, " + maxSeqNumber + "]");
            }

            var maxValue = maxSeqNumber - 2;
            if (options.MinSeqNumber < 5 || options.MinSeqNumber > maxValue)
            {
                throw new ApplicationException("MinSeqNumber is error. (range:[5, " + maxValue + "]");
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
                Thread.Sleep(500);
            }
        }


        public long NewLong()
        {
            return _SnowWorker.NextId();
        }
    }
}
