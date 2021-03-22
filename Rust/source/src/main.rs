mod yitgen;

use yitgen::contract::*;
use yitgen::gen::*;
use std::thread;
use chrono::Utc;
use std::sync::{Arc, Mutex};
use std::time::Duration;
use std::ops::Deref;

fn main() {
    println!("Hello, world! Rust");

    // 全局设置一次运行参数
    let mut options = IdGeneratorOptions::New(1);
    options.WorkerId = 1;
    options.WorkerIdBitLength = 1;
    options.SeqBitLength = 6;
    //... 可以继续设置其它 options 参数
    YitIdHelper::SetIdGenerator(options);

    // 以下开始测试生成数据，默认5W，单线程，可以修改 multiThread=true 启用多线程。
    loop {
        let mut i = 0;
        let mut id: i64 = 0;
        let multiThread = false;
        let start = Utc::now().timestamp_millis();

        while i < 50000 {
            i += 1;
            if multiThread { // 这是多线程
                thread::spawn(move || {
                    id = YitIdHelper::NextId();
                    println!("{}, id: {}", i, id);
                });
            } else { // 这是单线程
                id = YitIdHelper::NextId();
            }
        }

        println!("最后生成的id: {}", id);
        if !multiThread {
            // 多线程情况下，时间统计不准确
            let end = Utc::now().timestamp_millis();
            println!("单线程用时 {} ms", end - start);
        }

        thread::sleep(std::time::Duration::from_millis(1000));
    }
}

