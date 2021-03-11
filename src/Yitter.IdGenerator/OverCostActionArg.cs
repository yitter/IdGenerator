using System;
using System.Collections.Generic;
using System.Text;

namespace Yitter.IdGenerator
{
    public class OverCostActionArg
    {
        public int ActionType { get; set; }
        public long TimeTick { get; set; }
        public ushort WorkerId { get; set; }
        public int OverCostCountInOneTerm { get; set; }
        public int GenCountInOneTerm { get; set; }
        public int TermIndex { get; set; }
        
        public OverCostActionArg(ushort workerId, long timeTick, int actionType = 0, int overCostCountInOneTerm = 0, int genCountWhenOverCost = 0,int index=0)
        {
            ActionType = actionType;
            TimeTick = timeTick;
            WorkerId = workerId;
            OverCostCountInOneTerm = overCostCountInOneTerm;
            GenCountInOneTerm = genCountWhenOverCost;
            TermIndex = index;
        }
    }
}
