/*
* 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
*/
module gen

import contract

pub struct YitIdHelper {
mut:
	id_gen contract.IIdGenerator
}

pub fn (mut yih YitIdHelper) set_id_generator(options &contract.IdGeneratorOptions) {
	lock  {
		yih.id_gen = make_generator(options)
	}
}

pub fn (yih &YitIdHelper) next_id() u64 {
	return yih.id_gen.new_long()
}
