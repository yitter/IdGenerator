using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Yitter.IdGenerator;

namespace Yitter.OrgSystem.TestA
{
    class Program
    {
        // 测试参数（默认配置下，最佳性能是10W/s）
        static int genIdCount = 2;//5000;  // 计算ID数量（如果要验证50W效率，请将TopOverCostCount设置为20000或适当增加SeqBitLength）
        static short method = 1; // 1-漂移算法，2-传统算法


        static bool single = true;
        static bool outputLog = false;
        static IIdGenerator IdGen = null;
        static IList<GenTest> testList = new List<GenTest>();
        static bool checkResult = false;
        public static int Count = 0;
        static int workerCount = 1;


        static void Main(string[] args)
        {
            Console.WriteLine("Hello World! C#");

            var options = new IdGeneratorOptions()
            {
                Method = method,
                WorkerId = 1,

                WorkerIdBitLength = 6,
                SeqBitLength = 6,
                DataCenterIdBitLength = 10,
                TopOverCostCount = 2000,

                //TimestampType = 1,

                // MinSeqNumber = 1,
                // MaxSeqNumber = 200,

                // BaseTime = DateTime.Now.AddYears(-10),
            };

            IdGen = new DefaultIdGenerator(options);
            GenTest genTest = new GenTest(IdGen, genIdCount, options.WorkerId);

            // 首先测试一下 IdHelper 方法，获取单个Id
            YitIdHelper.SetIdGenerator(options);
            long newId = YitIdHelper.NextId();
            Console.WriteLine("=====================================");
            Console.WriteLine("这是用方法 " + method + " 生成的 Id：" + newId);

            while (true)
            {
                RunSingle();
                //CallDll();
                //Go(options);
                Thread.Sleep(1000); // 每隔1秒执行一次Go
            }
        }

        private static void RunSingle()
        {
            DateTime start = DateTime.Now;

            for (int i = 0; i < genIdCount; i++)
            {
                var id = IdGen.NewLong();
            }

            DateTime end = DateTime.Now;
            Console.WriteLine($"++++++++++++++++++++++++++++++++++++++++, total: {(end - start).TotalMilliseconds} ms");
            Interlocked.Increment(ref Program.Count);
        }

        private static void Go(IdGeneratorOptions options)
        {
            Count = 0;
            testList = new List<GenTest>();

            // ++++++++++++++++++++++++++++++++
            if (single)
            {
                if (IdGen == null)
                {
                    IdGen = new DefaultIdGenerator(options);
                }

                if (outputLog)
                {
                    IdGen.GenIdActionAsync = (arg =>
                    {
                        if (arg.ActionType == 1)
                        {
                            Console.WriteLine($">>>> {arg.WorkerId}：开始：{DateTime.Now.ToString("mm:ss:fff")}, 周期次序：{arg.TermIndex}");
                        }
                        else if (arg.ActionType == 2)
                        {
                            Console.WriteLine($"<<<< {arg.WorkerId}：结束：{DateTime.Now.ToString("mm:ss:fff")}，漂移 {arg.OverCostCountInOneTerm} 次，产生 {arg.GenCountInOneTerm} 个, 周期次序：{arg.TermIndex}");
                        }
                        if (arg.ActionType == 8)
                        {
                            Console.WriteLine($"---- {arg.WorkerId}：AA结束：{DateTime.Now.ToString("mm:ss:fff")}，时间回拨");
                        }
                    });
                }

                for (int i = 1; i < workerCount + 1; i++)
                {
                    Console.WriteLine("Gen：" + i);
                    var test = new GenTest(IdGen, genIdCount, i);
                    testList.Add(test);
                    // test.GenId();
                    test.GenStart();
                }
            }
            else
            {
                for (int i = 1; i < workerCount + 1; i++)
                {
                    IdGeneratorOptions newOptions = new IdGeneratorOptions()
                    {
                        WorkerId = (ushort)i, // workerId 不能设置为0
                        WorkerIdBitLength = options.WorkerIdBitLength,
                        SeqBitLength = options.SeqBitLength,
                        MinSeqNumber = options.MinSeqNumber,
                        MaxSeqNumber = options.MaxSeqNumber,
                        Method = options.Method,
                        //TimestampType = 1,
                    };

                    Console.WriteLine("Gen：" + i);
                    var idGen2 = new DefaultIdGenerator(newOptions);
                    var test = new GenTest(idGen2, genIdCount, i);

                    if (outputLog)
                    {
                        idGen2.GenIdActionAsync = (arg =>
                        {
                            Console.WriteLine($"{DateTime.Now.ToString("mm:ss:fff")} {arg.WorkerId} 漂移了 {arg.OverCostCountInOneTerm}, 顺序：{arg.TermIndex}");
                        });
                    }

                    testList.Add(test);
                    // test.GenId();
                    test.GenStart();
                }
            }

            while (Count < workerCount)
            {
                //Console.WriteLine("Count：" + Count);
                Thread.Sleep(1000);
            }
            //Console.WriteLine("Count：" + Count);

            if (!checkResult)
            {
                return;
            }

            var dupCount = 0;
            foreach (var id in testList[0].idList)
            {
                if (id == 0)
                {
                    Console.WriteLine("############### 错误的ID：" + id + "###########");
                }

                for (int j = 1; j < testList.Count; j++)
                {
                    if (testList[j].idList.Contains(id))
                    {
                        dupCount++;
                        Console.WriteLine("xxxxxxxxxx 重复的ID：" + id);
                    }
                }
            }

            if (dupCount > 0)
            {
                Console.WriteLine($"重复数量：{dupCount}");
            }

        }

    }
}
