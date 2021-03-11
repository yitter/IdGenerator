using System;

namespace Yitter.IdGenerator
{
    public interface IIdGenerator
    {
        Action<OverCostActionArg> GenIdActionAsync { get; set; }

        long NewLong();
    }
}
