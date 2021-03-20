/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package contract

type IdGeneratorOptions struct {
	Method            uint16 	// 雪花计算方法,（1-漂移算法|2-传统算法），默认1
	BaseTime          int64		// 基础时间，不能超过当前系统时间
	WorkerId          uint16	// 机器码，与 WorkerIdBitLength 有关系
	WorkerIdBitLength byte		// 机器码位长，范围：1-21（要求：序列数位长+机器码位长不超过22）
	SeqBitLength      byte		// 序列数位长，范围：2-21（要求：序列数位长+机器码位长不超过22）
	MaxSeqNumber      uint32	// 最大序列数（含），（由SeqBitLength计算的最大值）
	MinSeqNumber      uint32	// 最小序列数（含），默认5，不小于1，不大于MaxSeqNumber
	TopOverCostCount  uint		// 最大漂移次数（含），默认2000，推荐范围500-10000（与计算能力有关）
}

func NewIdGeneratorOptions(workerId uint16) *IdGeneratorOptions {
	return &IdGeneratorOptions{
		Method:            1,
		WorkerId:          workerId,
		BaseTime:          1582136402000,
		WorkerIdBitLength: 6,
		SeqBitLength:      6,
		MaxSeqNumber:      0,
		MinSeqNumber:      5,
		TopOverCostCount:  2000,
	}
}
