/*
* 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
*/
module gen

import contract
import core
import time

pub struct DefaultIdGenerator {
mut:
	options     &contract.IdGeneratorOptions
	snow_worker &contract.ISnowWorker
}

pub fn make_generator(options &contract.IdGeneratorOptions) &DefaultIdGenerator {
	min_time := i64(631123200000)
	if options.base_time < min_time || options.base_time > time.now().unix_time_milli() {
		panic('base_time error.')
	}

	if options.seq_bitlength + options.workerid_bitlength > 22 {
		panic('error：workerid_bitlength + seq_bitlength <= 22')
	}

	max_workerid_number := 1 << options.workerid_bitlength - 1
	if options.worker_id > max_workerid_number {
		panic('WorkerId error. (range:[1, ' + max_workerid_number.str() + ']')
	}

	if options.seq_bitlength < 2 || options.seq_bitlength > 21 {
		panic('seq_bitlength error. (range:[2, 21])')
	}

	max_seqnumber := 1 << options.seq_bitlength - 1
	if options.max_seqnumber > max_seqnumber {
		panic('MaxSeqNumber error. (range:[1, ' + max_seqnumber.str() + ']')
	}

	if options.min_seqnumber > max_seqnumber {
		panic('MinSeqNumber error. (range:[1, ' + max_seqnumber.str() + ']')
	}

	match options.method {
		1 {
			return &DefaultIdGenerator{
				options: options
				snow_worker: core.make_sf_m1(options)
			}
		}
		2 {
			return &DefaultIdGenerator{
				options: options
				snow_worker: core.make_sf_m2(options)
			}
		}
		else {
			return &DefaultIdGenerator{
				options: options
				snow_worker: core.make_sf_m1(options)
			}
		}
	}
}

pub fn (mut dig DefaultIdGenerator) new_long() u64 {
	return dig.snow_worker.next_id()
}
