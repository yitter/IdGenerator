/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
use super::super::contract::*;
use super::super::core::*;
use super::*;
use std::sync::Mutex;
use std::sync::Arc;
use lazy_static::lazy_static;


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

    #[no_mangle]
    pub fn NextId() -> i64 {
        let mut idgenArc = YitIdHelper::IdGenInstance();
        let mut idgen = idgenArc.lock().unwrap();
        idgen.Worker.NextId()
    }
}
