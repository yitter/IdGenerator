using System;
using System.Collections.Generic;
using System.Text;

namespace Yitter.IdGenerator
{
    internal interface ISnowWorker
    {
        Action<OverCostActionArg> GenAction { get; set; }

        long NextId();
    }
}
