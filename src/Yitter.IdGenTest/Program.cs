using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using Yitter.IdGenerator;

namespace Yitter.OrgSystem.TestA
{
    class Program
    {
        static int workerCount = 1;
        static int genIdCount = 10000;  // 计算ID数量
        static short method = 1; // 1-漂移算法，2-传统算法
        static bool single = true;
        static bool outputLog = true;
        static IIdGenerator IdGen = null;
        static IList<GenTest> testList = new List<GenTest>();
        static bool checkResult = false;
        public static int Count = 0;


        static void Main(string[] args)
        {
            while (true)
            {
                Go();
                Thread.Sleep(5000);
                Console.WriteLine("Hello World!");
            }
        }

        private static void Go()
        {
            Count = 0;
            testList = new List<GenTest>();

            var newConfig = new IdGeneratorOptions()
            {
                Method = method,
                StartTime = DateTime.Now.AddYears(-1),

                //TopOverCostCount = 1000,
                WorkerIdBitLength = 6,
                SeqBitLength = 6,

                //MinSeqNumber = 11,
                //MaxSeqNumber = 200,
            };

            // ++++++++++++++++++++++++++++++++
            if (single)
            {
                newConfig.WorkerId = 1;
                IdGeneratorOptions options1 = (newConfig);
                if (IdGen == null)
                {
                    IdGen = new YidGenerator(options1);
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
                    test.GenId();
                }
            }
            else
            {
                for (int i = 1; i < workerCount + 1; i++)
                {
                    IdGeneratorOptions options =
                  new IdGeneratorOptions()
                  {
                      WorkerId = (ushort)i, // workerId 不能设置为0
                      WorkerIdBitLength = newConfig.WorkerIdBitLength,
                      SeqBitLength = newConfig.SeqBitLength,
                      MinSeqNumber = newConfig.MinSeqNumber,
                      MaxSeqNumber = newConfig.MaxSeqNumber,
                      Method = newConfig.Method,
                  };

                    Console.WriteLine("Gen：" + i);
                    var idGen2 = new YidGenerator(options);
                    var test = new GenTest(idGen2, genIdCount, i);

                    if (outputLog)
                    {
                        idGen2.GenIdActionAsync = (arg =>
                        {
                            Console.WriteLine($"{DateTime.Now.ToString("mm:ss:fff")} {arg.WorkerId} 漂移了 {arg.OverCostCountInOneTerm}, 顺序：{arg.TermIndex}");
                        });
                    }

                    testList.Add(test);
                    test.GenId();
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
