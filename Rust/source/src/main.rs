mod idgen;

use chrono::Utc;
use idgen::*;
use std::thread;
use std::time::Duration;

fn main() {
    println!("Hello, world! Rust");

    // 总执行次数
    let times = 500000;

    // 是否启用多线程测试
    let multiThread = false;

    // 全局设置一次运行参数
    let mut options = IdGeneratorOptions::New(1);
    options.WorkerIdBitLength = 6;
    options.SeqBitLength = 10;
    //... 可以继续设置其它 options 参数
    YitIdHelper::SetIdGenerator(options);

    set_redis();

    // 以下开始测试生成数据，默认5W，单线程，可以修改 multiThread=true 启用多线程。
    loop {
        let mut i = 0;
        let mut id: i64 = 0;
        let start = Utc::now().timestamp_millis();

        while i < times {
            i += 1;
            id = YitIdHelper::NextId();
            println!("id: {}", id);

            // if multiThread { // 这是多线程
            //     thread::spawn(move || {
            //         id = YitIdHelper::NextId();
            //         println!("{}, id: {}", i, id);
            //     });
            // } else { // 这是单线程
            //     id = YitIdHelper::NextId();
            // }
        }

        let end = Utc::now().timestamp_millis();
        println!("单线程用时 {} ms", end - start);

        // println!("最后生成的id: {}", id);
        // if !multiThread {
        //     // 多线程情况下，时间统计不准确
        //     let end = Utc::now().timestamp_millis();
        //     println!("单线程用时 {} ms", end - start);
        // }

        thread::sleep(std::time::Duration::from_millis(1000));
    }
}

fn set_redis() {

    // match simple_redis::create("redis://127.0.0.1:6379/") {
    //     Ok(mut client) =>  {
    //         println!("Created Redis Client");
    //
    //         match client.set("my_key", "my_value") {
    //             Err(error) => println!("Unable to set value in Redis: {}", error),
    //             _ => println!("Value set in Redis")
    //         };
    //
    //         match client.quit() {
    //             Err(error) => println!("Error: {}", error),
    //             _ => println!("Connection Closed.")
    //         }
    //     },
    //     Err(error) => println!("Unable to create Redis client: {}", error)
    // }
}
