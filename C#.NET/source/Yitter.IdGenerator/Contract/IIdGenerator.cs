/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 * 版权协议：MIT
 * 版权说明：只要保留本版权，你可以免费使用、修改、分发本代码。
 * 免责条款：任何因为本代码产生的系统、法律、政治、宗教问题，均与版权所有者无关。
 * 
 */

using System;

namespace Yitter.IdGenerator
{
    public interface IIdGenerator
    {
        /// <summary>
        /// 生成过程中产生的事件
        /// </summary>
        //Action<OverCostActionArg> GenIdActionAsync { get; set; }

        /// <summary>
        /// 生成新的long型Id
        /// </summary>
        /// <returns></returns>
        long NewLong();

        // Guid NewGuid();
    }
}
