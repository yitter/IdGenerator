/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
use super::super::contract::*;
use super::super::core::*;
use super::*;
use std::sync::Mutex;
use std::sync::Arc;

pub struct YitIdHelper;

static mut idGenInstance: Option<Arc<Mutex<DefaultIdGenerator>>> = None;

impl YitIdHelper {
    fn IdGenInstance() -> Arc<Mutex<DefaultIdGenerator>> {
        unsafe {
            idGenInstance.get_or_insert_with(|| {
                Arc::new(Mutex::new(DefaultIdGenerator::Default()))
            }).clone()
        }
    }

    pub fn SetIdGenerator(options: IdGeneratorOptions) {
        let mut idgenArc = YitIdHelper::IdGenInstance();
        let mut idgen = idgenArc.lock().unwrap();
        idgen.Worker.SetOptions(options);
    }

    pub fn SetWorkerId(workerId: u32) {
        let mut idgenArc = YitIdHelper::IdGenInstance();
        let mut idgen = idgenArc.lock().unwrap();
        let mut options = IdGeneratorOptions::New(workerId);
        idgen.Worker.SetOptions(options);
    }

    pub fn NextId() -> i64 {
        let mut idgenArc = YitIdHelper::IdGenInstance();
        let mut idgen = idgenArc.lock().unwrap();
        idgen.Worker.NextId()
    }
}
