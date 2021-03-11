/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 * 版权协议：MIT
 * 版权说明：只要保留本版权，你可以免费使用、修改、分发本代码。
 * 免责条款：任何因为本代码产生的系统、法律、政治、宗教问题，均与版权所有者无关。
 * 
 */

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
