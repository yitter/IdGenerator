mod yitgen;

use yitgen::gen::YitIdHelper;
use yitgen::contract::*;

#[macro_use]
extern crate lazy_static;
extern crate redis;
extern crate libc;

use redis::Commands;

use libc::{c_char, uint32_t};
use std::ffi::{CStr, CString};
use std::str;

lazy_static! {
        //static ref TestValue: Vec<i32> = vec!(0);
        // static ref MAP: HashMap<u32, String> = HashMap::new();
 }


// // #[export_name = "SetIdGenerator"]
// #[no_mangle]
// pub extern "C" fn SetIdGenerator(options: IdGeneratorOptions) {
//     YitIdHelper::SetIdGenerator(options);
// }

#[no_mangle]
pub extern "C" fn SetOptions(workerId: u32, workerIdBitLength: u8, seqBitLength: u8) {
    let mut options = IdGeneratorOptions::New(1);
    options.WorkerIdBitLength = workerIdBitLength;
    options.SeqBitLength = seqBitLength;
    YitIdHelper::SetIdGenerator(options);
}

#[no_mangle]
pub extern "C" fn SetWorkerId(workerId: u32) {
    YitIdHelper::SetWorkerId(workerId);
}

#[no_mangle]
pub extern "C" fn NextId() -> i64 {
    YitIdHelper::NextId()
}

// static mut TestValue: i32 = 0;
// #[no_mangle]
// pub extern "C" fn Test() -> i32 {
//     unsafe {
//         TestValue += 1;
//         return TestValue;
//     }
// }

// #[no_mangle]
// pub extern "C"
// fn GetWorkerId(ip: *const c_char, port: i32) -> redis::RedisResult<isize> {
//     // let c_str = unsafe {
//     //     assert!(!ip.is_null());
//     //     CStr::from_ptr(ip)
//     // };
//     //
//     // let r_str = c_str.to_str();
//
//     // connect to redis
//     // let client = redis::Client::open(format!("redis://{}:{}/", String::from(r_str).to_string(), port))?;
//     let client = redis::Client::open(format!("redis://localhost:{}/", port))?;
//
//     let mut con = client.get_connection()?;
//     // throw away the result, just make sure it does not fail
//     unsafe {
//         let _: () = con.set("my_key111", TestValue.clone())?;
//     }
//     con.get("my_key")
// // read back the key and return it.  Because the return value
// // from the function is a result for integer this will automatically
// // convert into one.
// //
//
// // match simple_redis::create(&format!("redis://{}:{}/", ip, port)) {
// //     Ok(mut client) => {
// //         println!("Created Redis Client");
// //
// //         let valueString = TestValue.to_string();
// //         let valueString2 = (*TestValue).to_string();
// //
// //         match client.set("my_key", valueString) {
// //             Err(error) => println!("Unable to set value in Redis: {}", error),
// //             _ => println!("Value set in Redis")
// //         };
// //
// //         match client.set("my_key2", valueString2) {
// //             Err(error) => println!("Unable to set value in Redis: {}", error),
// //             _ => println!("Value set in Redis")
// //         };
// //
// //         match client.quit() {
// //             Err(error) => println!("Error: {}", error),
// //             _ => println!("Connection Closed.")
// //         }
// //     }
// //     Err(error) => println!("Unable to create Redis client: {}", error)
// // }
//
// //return 1;
// }

// build-win-x64: cargo build --release
// build-linux-x64: cargo build --target x86_64-unknown-linux-musl --release
