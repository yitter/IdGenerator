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

namespace Yitter.IdGenerator
{
    /// <summary>
    /// Id生成时回调参数
    /// </summary>
    public class OverCostActionArg
    {
        /// <summary>
        /// 事件类型
        /// 1-开始，2-结束，8-漂移
        /// </summary>
        public virtual int ActionType { get; set; }
        /// <summary>
        /// 时间戳
        /// </summary>
        public virtual long TimeTick { get; set; }
        /// <summary>
        /// 机器码
        /// </summary>
        public virtual ushort WorkerId { get; set; }
        /// <summary>
        /// 漂移计算次数
        /// </summary>
        public virtual int OverCostCountInOneTerm { get; set; }
        /// <summary>
        /// 漂移期间生产ID个数
        /// </summary>
        public virtual int GenCountInOneTerm { get; set; }
        /// <summary>
        /// 漂移周期
        /// </summary>
        public virtual int TermIndex { get; set; }

        public OverCostActionArg(ushort workerId, long timeTick, int actionType = 0, int overCostCountInOneTerm = 0, int genCountWhenOverCost = 0, int index = 0)
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
