/*
* 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
*/
module core

import contract
import time

pub struct SnowWorkerM1 {
mut:
	method              u16  // 雪花计算方法,（1-漂移算法|2-传统算法），默认1
	base_time           i64  // 基础时间，不能超过当前系统时间
	worker_id           u16  // 机器码，与 workerid_bitlength 有关系
	workerid_bitlength  byte // 机器码位长，范围：1-21（要求：序列数位长+机器码位长不超过22）
	seq_bitlength       byte // 序列数位长，范围：2-21（要求：序列数位长+机器码位长不超过22）
	max_seqnumber       u32  // 最大序列数（含），（由seq_bitlength计算的最大值）
	min_seqnumber       u32  // 最小序列数（含），默认5，不小于1，不大于max_seqnumber
	top_over_cost_count u32  // 最大漂移次数（含），默认2000，推荐范围500-10000（与计算能力有关）

	timestamp_shift         byte
	current_seqnumber       u32
	last_time_tick          i64
	turn_back_timetick      i64
	turnback_index          byte
	is_over_cost            bool
	overcostcount_inoneterm u32
	// gencount_inoneterm      u32
	// term_index              u32
}

pub fn make_sf_m1(options &contract.IdGeneratorOptions) &contract.ISnowWorker {
	worker_id := options.worker_id

	mut workerid_bitlength := byte(6)
	if options.workerid_bitlength != 0 {
		workerid_bitlength = options.workerid_bitlength
	}
	mut seq_bitlength := byte(6)
	if options.seq_bitlength != 0 {
		seq_bitlength = options.seq_bitlength
	}
	mut max_seqnumber := u32(0)
	if options.max_seqnumber > 0 {
		max_seqnumber = options.max_seqnumber
	} else {
		max_seqnumber = (1 << options.seq_bitlength) - 1
	}
	min_seqnumber := options.min_seqnumber
	top_over_cost_count := options.top_over_cost_count

	mut base_time := i64(0)
	if options.base_time != 0 {
		base_time = options.base_time
	} else {
		base_time = 1582136402000
	}
	timestamp_shift := byte(options.workerid_bitlength + options.seq_bitlength)
	current_seqnumber := options.min_seqnumber
	return &SnowWorkerM1{
		base_time: base_time
		worker_id: worker_id
		workerid_bitlength: workerid_bitlength
		seq_bitlength: seq_bitlength
		max_seqnumber: max_seqnumber
		min_seqnumber: min_seqnumber
		top_over_cost_count: top_over_cost_count
		timestamp_shift: timestamp_shift
		current_seqnumber: current_seqnumber
	}
}

// fn (m1 SnowWorkerM1) do_gen_id_action(arg &contract.over_cost_action_arg) {

// }

// fn (m1 &SnowWorkerM1) begin_over_cost_action(use_time_tick i64) {

// }

// fn (mut m1 SnowWorkerM1) end_over_cost_action() {
// 	if m1.term_index > 10000 {
// 		m1.term_index = 0
// 	}
// }

// fn (m1 &SnowWorkerM1) begin_turn_back_action(use_time_tick i64) {

// }

// fn (m1 &SnowWorkerM1) end_turn_back_action(use_time_tick i64) {

// }

fn (mut m1 SnowWorkerM1) next_over_cost_id() u64 {
	current_time_tick := m1.get_current_time_tick()
	if current_time_tick > m1.last_time_tick {
		// m1.end_over_cost_action()
		m1.last_time_tick = current_time_tick
		m1.current_seqnumber = m1.min_seqnumber
		m1.is_over_cost = false
		m1.overcostcount_inoneterm = 0
		// m1.gencount_inoneterm = 0
		return m1.calc_id()
	}
	if m1.overcostcount_inoneterm >= m1.top_over_cost_count {
		// m1.end_over_cost_action()
		m1.last_time_tick = m1.get_next_time_tick()
		m1.current_seqnumber = m1.min_seqnumber
		m1.is_over_cost = false
		m1.overcostcount_inoneterm = 0
		// m1.gencount_inoneterm = 0
		return m1.calc_id()
	}
	if m1.current_seqnumber > m1.max_seqnumber {
		m1.last_time_tick++
		m1.current_seqnumber = m1.min_seqnumber
		m1.is_over_cost = true
		m1.overcostcount_inoneterm++
		// m1.gencount_inoneterm++

		return m1.calc_id()
	}
	// m1.gencount_inoneterm++
	return m1.calc_id()
}

fn (mut m1 SnowWorkerM1) next_normal_id() u64 {
	current_time_tick := m1.get_current_time_tick()
	if current_time_tick < m1.last_time_tick {
		if m1.turn_back_timetick < 1 {
			m1.turn_back_timetick = m1.last_time_tick - 1
			m1.turnback_index++
			// 每毫秒序列数的前5位是预留位，0用于手工新值，1-4是时间回拨次序
			// 最多4次回拨（防止回拨重叠）
			if m1.turnback_index > 4 {
				m1.turnback_index = 1
			}
			// m1.begin_turn_back_action(m1.turn_back_timetick)
		}
		return m1.calc_turn_back_id()
	}
	// 时间追平时，turn_back_timetick清零
	if m1.turn_back_timetick > 0 {
		// m1.end_turn_back_action(m1.turn_back_timetick)
		m1.turn_back_timetick = 0
	}
	if current_time_tick > m1.last_time_tick {
		m1.last_time_tick = current_time_tick
		m1.current_seqnumber = m1.min_seqnumber
		return m1.calc_id()
	}
	if m1.current_seqnumber > m1.max_seqnumber {
		// m1.begin_over_cost_action(current_time_tick)
		// m1.term_index++
		m1.last_time_tick++
		m1.current_seqnumber = m1.min_seqnumber
		m1.is_over_cost = true
		m1.overcostcount_inoneterm = 1
		// m1.gencount_inoneterm = 1

		return m1.calc_id()
	}
	return m1.calc_id()
}

fn (mut m1 SnowWorkerM1) calc_id() u64 {
	result := u64(m1.last_time_tick << m1.timestamp_shift) | u64(m1.worker_id << m1.seq_bitlength) | u64(m1.current_seqnumber)
	m1.current_seqnumber++
	return result
}

fn (mut m1 SnowWorkerM1) calc_turn_back_id() u64 {
	result := u64(m1.turn_back_timetick << m1.timestamp_shift) | u64(m1.worker_id << m1.seq_bitlength) | u64(m1.turnback_index)
	m1.turn_back_timetick--
	return result
}

fn (m1 &SnowWorkerM1) get_current_time_tick() i64 {
	return i64(time.now().unix_time_milli()) - m1.base_time
}

fn (m1 &SnowWorkerM1) get_next_time_tick() i64 {
	mut temp_time_ticker := m1.get_current_time_tick()
	for temp_time_ticker <= m1.last_time_tick {
		temp_time_ticker = m1.get_current_time_tick()
	}
	return temp_time_ticker
}

pub fn (mut m1 SnowWorkerM1) next_id() u64 {
	mut id := u64(0)
	lock  {
		if m1.is_over_cost {
			id = m1.next_over_cost_id()
		} else {
			id = m1.next_normal_id()
		}
	}
	return id
}
