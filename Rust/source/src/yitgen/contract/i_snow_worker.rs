/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
pub trait ISnowWorker {
    fn NextId(&self) -> i64;
}