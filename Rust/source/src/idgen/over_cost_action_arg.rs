/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 */
pub struct OverCostActionArg {
    ActionType: u32,
    TimeTick: i64,
    WorkerId: u16,
    OverCostCountInOneTerm: i32,
    GenCountInOneTerm: i32,
    TermIndex: i32,
}