module gen

import contract

pub struct YitIdHelper  {
mut:
	id_gen contract.IIdGenerator
}

pub fn (yih &YitIdHelper) next_id() u64 {
	return yih.id_gen.new_long()
}