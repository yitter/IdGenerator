/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
pub struct OverCostActionArg {
    ActionType: u32,
    TimeTick: u64,
    WorkerId: u16,
    OverCostCountInOneTerm: u32,
    GenCountInOneTerm: u32,
    TermIndex: u32,
}