/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
use std::{thread, time};
use std::net::UdpSocket;
use chrono::Utc;
use std::sync::Mutex;
use std::sync::Arc;
use std::borrow::BorrowMut;
use crate::idgen::*;

// static mut instance2: Option<Arc<Mutex<SnowWorkerM1>>> = None;

pub struct DefaultIdGenerator {
    pub Worker: SnowWorkerM1,
}

impl DefaultIdGenerator {
    pub fn Default() -> DefaultIdGenerator {
        DefaultIdGenerator { Worker: SnowWorkerM1::Default() }
    }
}
