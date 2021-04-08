/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.contract.OverCostActionArg;

/**
 * Id生成时回调参数
 */
class OverCostActionArg {

    /**
     * 事件类型
     * 1-开始，2-结束，8-漂移
     */
    int ActionType = 0;

    /**
     * 时间戳
     */
    long TimeTick = 0;

    /**
     * 机器码
     */
    short WorkerId = 0;

    /**
     *
     */
    int OverCostCountInOneTerm = 0;

    /**
     * 漂移期间生产ID个数
     */
    int GenCountInOneTerm = 0;

    /**
     * 漂移周期
     */
    int TermIndex = 0;

    this(short workerId, long timeTick, int actionType, int overCostCountInOneTerm, 
            int genCountWhenOverCost, int index) {
        ActionType = actionType;
        TimeTick = timeTick;
        WorkerId = workerId;
        OverCostCountInOneTerm = overCostCountInOneTerm;
        GenCountInOneTerm = genCountWhenOverCost;
        TermIndex = index;
    }
}
