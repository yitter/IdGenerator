mod yitgen;
use yitgen::gen::YitIdHelper;
use yitgen::contract::*;

// #[export_name = "SetIdGenerator"]
#[no_mangle]
pub extern "C" fn SetIdGenerator(options: IdGeneratorOptions) {
    YitIdHelper::SetIdGenerator(options);
}

#[no_mangle]
pub extern "C" fn NextId() -> i64 {
    YitIdHelper::NextId()
}

