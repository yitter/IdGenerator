/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package idgen

import (
	"../contract"
	"../core"
	"math"
	"time"
)

type DefaultIdGenerator struct {
	Options              *contract.IdGeneratorOptions
	SnowWorker           contract.ISnowWorker
	IdGeneratorException contract.IdGeneratorException
}

func NewDefaultIdGenerator(Options *contract.IdGeneratorOptions) *DefaultIdGenerator {
	if Options == nil {
		panic("dig.Options error.")
	}

	var minTime = time.Now().AddDate(-50, 0, 0).UnixNano() / 1e6
	if minTime == 0 || Options.BaseTime < minTime || Options.BaseTime > time.Now().UnixNano()/1e6 {
		panic("BaseTime error.")
	}

	if Options.SeqBitLength+Options.WorkerIdBitLength > 22 {
		panic("error：WorkerIdBitLength + SeqBitLength <= 22")
	}

	maxWorkerIdNumber := uint16(math.Pow(float64(2), float64(Options.WorkerIdBitLength))) - 1
	if Options.WorkerId > maxWorkerIdNumber {
		panic("WorkerId error. (range:[1, "+ string(maxWorkerIdNumber)+ "]")
	}

	if Options.SeqBitLength < 2 || Options.SeqBitLength > 21 {
		panic("SeqBitLength error. (range:[2, 21])")
	}

	maxSeqNumber := uint32(math.Pow(2, float64(Options.SeqBitLength))) - 1
	if Options.MaxSeqNumber > maxSeqNumber {
		panic("MaxSeqNumber error. (range:[1, "+ string(maxSeqNumber)+ "]")
	}

	if Options.MinSeqNumber > maxSeqNumber {
		panic("MinSeqNumber error. (range:[1, "+ string(maxSeqNumber)+ "]")
	}

	var snowWorker contract.ISnowWorker

	switch Options.Method {
	case 1:
		snowWorker = core.NewSnowWorkerM1(Options)
	case 2:
		snowWorker = core.NewSnowWorkerM2(Options)
	default:
		snowWorker = core.NewSnowWorkerM1(Options)
	}

	if Options.Method == 1 {
		time.Sleep(time.Duration(500) * time.Microsecond)
	}
	return &DefaultIdGenerator{
		Options:    Options,
		SnowWorker: snowWorker,
	}
}

func (dig DefaultIdGenerator) NewLong() uint64 {
	return dig.SnowWorker.NextId()
}
