/*
* 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
*/
module core

import contract

struct SnowWorkerM2 {
	SnowWorkerM1
}

pub fn make_sf_m2(options &contract.IdGeneratorOptions) &contract.ISnowWorker {
	m1 := make_sf_m1(options)
	if m1 is SnowWorkerM1 {
		return &SnowWorkerM2{m1}
	}
	return &SnowWorkerM2{}
}

pub fn (mut m2 SnowWorkerM2) next_id() u64 {
	mut id := u64(0)
	lock  {
		mut current_time_tick := m2.get_current_time_tick()
		if m2.last_time_tick == current_time_tick {
			m2.current_seqnumber = (m2.current_seqnumber + 1) & m2.max_seqnumber
			if m2.current_seqnumber == 0 {
				m2.current_seqnumber = m2.min_seqnumber
				current_time_tick = m2.get_next_time_tick()
			}
		} else {
			m2.current_seqnumber = m2.min_seqnumber
		}
		if current_time_tick < m2.last_time_tick {
			println('Time error for ' + (m2.last_time_tick - current_time_tick).str() +
				' milliseconds')
		}
		m2.last_time_tick = current_time_tick
		id = u64(current_time_tick << m2.timestamp_shift) | u64(m2.worker_id << m2.seq_bitlength) | u64(m2.current_seqnumber)
	}
	return id
}
