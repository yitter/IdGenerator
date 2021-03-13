using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Yitter.IdGenerator;

namespace Yitter.OrgSystem.TestA
{
    public class GenTest
    {
        private IIdGenerator IdGen;
        private Hashtable ids = new Hashtable();
        public IList<long> idList = new List<long>();
        private int GenIdCount;
        private int WorkerId;
            

        public GenTest(IIdGenerator idGen, int genIdCount, int workerId)
        {
            GenIdCount = genIdCount;
            IdGen = idGen;
            WorkerId = workerId;
        }

        //public void GenId()
        //{
        //    Thread t = new Thread(new ThreadStart(Gen1Start));
        //    t.Start();
        //}

        public void GenStart()
        {
            DateTime start = DateTime.Now;

            for (int i = 0; i < GenIdCount; i++)
            {
                var id = IdGen.NewLong();
                //ids.Add(id, i);
                //idList.Add(id);
            }

            DateTime end = DateTime.Now;
            Console.WriteLine($"++++++++++++++++++++++++++++++++++++++++WorkerId: {WorkerId}, total: {(end - start).TotalSeconds} s");
            Interlocked.Increment(ref Program.Count);
        }
    }
}
