module contract

pub struct IdGeneratorOptions {
pub mut:
	method              u16  = 1 // 雪花计算方法,（1-漂移算法|2-传统算法），默认1
	base_time           i64  = 1582136402000 // 基础时间，不能超过当前系统时间
	worker_id           u16  = 1 // 机器码，必须由外部设定，最大值 2^WorkerIdBitLength-1
	workerid_bitlength  byte = 6 // 机器码位长，默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
	seq_bitlength       byte = 6 // 序列数位长，默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
	max_seqnumber       u32  = 0 // 最大序列数（含），设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
	min_seqnumber       u32  = 5 // 最小序列数（含），默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
	top_over_cost_count u32  = 2000 // 最大漂移次数（含），默认2000，推荐范围 500-20000（与计算能力有关）
}
