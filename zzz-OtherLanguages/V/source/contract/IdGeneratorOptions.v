module contract

pub struct IdGeneratorOptions {
pub mut:
	method              u16  = 1 // 雪花计算方法,（1-漂移算法|2-传统算法），默认1
	base_time           i64  = 1582136402000 // 基础时间，不能超过当前系统时间
	worker_id           u16  = 1 // 机器码，与 workerid_bitlength 有关系
	workerid_bitlength  byte = 6 // 机器码位长，范围：1-21（要求：序列数位长+机器码位长不超过22）
	seq_bitlength       byte = 6 // 序列数位长，范围：2-21（要求：序列数位长+机器码位长不超过22）
	max_seqnumber       u32  = 0 // 最大序列数（含），（由seq_bitlength计算的最大值）
	min_seqnumber       u32  = 0 // 最小序列数（含），默认5，不小于1，不大于max_seqnumber
	top_over_cost_count u32  = 2000 // 最大漂移次数（含），默认2000，推荐范围500-10000（与计算能力有关）
}
