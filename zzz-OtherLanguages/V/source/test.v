module main

import time
import contract
import gen

__global ( idgen gen.YitIdHelper )

fn main() {
	idgen = gen.YitIdHelper{
		id_gen: gen.make_generator(&contract.IdGeneratorOptions{})
	}
	// 方法一：直接采用默认方法生成一个Id
	println(idgen.next_id())

	idgen.set_id_generator(&contract.IdGeneratorOptions{
		method: 1
		base_time: 1582136402000
		workerid_bitlength: 6
		seq_bitlength: 10
	})

	times := 50000

	for {
		begin := time.now().unix_time_milli()
		for i := 0; i < times; i++ {
			idgen.next_id()
		}
		end := time.now().unix_time_milli()
		println('漂移，总共：' + times.str() + '，' + (end - begin).str() + ' ms')
		time.sleep(1 * time.second)
	}
}
