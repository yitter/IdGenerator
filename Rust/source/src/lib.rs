mod idgen;

#[macro_use]
// extern crate lazy_static;
extern crate libc;

use std::ffi::{CStr, CString};
use std::str;
pub use idgen::*;

// #[export_name = "SetIdGenerator"]
#[no_mangle]
pub extern "C" fn SetIdGenerator(options: IdGeneratorOptions) {
    YitIdHelper::SetIdGenerator(options);
}

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

// build-win-x64: cargo build --release
// build-linux-x64: cargo build --target x86_64-unknown-linux-musl --release
