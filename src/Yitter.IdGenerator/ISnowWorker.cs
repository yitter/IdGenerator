using System;
using System.Collections.Generic;
using System.Text;
using static Yitter.IdGenerator.IIdGenerator;

namespace Yitter.IdGenerator
{
    internal interface ISnowWorker
    {
        Action<OverCostActionArg> GenAction { get; set; }

        long NextId();
    }
}
