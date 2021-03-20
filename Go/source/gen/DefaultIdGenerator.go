/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package gen

import (
	"math"
	"time"
	"yitidgen/contract"
	"yitidgen/core"
)

type DefaultIdGenerator struct {
	Options              *contract.IdGeneratorOptions
	SnowWorker           contract.ISnowWorker
	IdGeneratorException contract.IdGeneratorException
}

func NewDefaultIdGenerator(options *contract.IdGeneratorOptions) *DefaultIdGenerator {
	if options == nil {
		panic("dig.Options error.")
	}

	var minTime = time.Now().AddDate(-50, 0, 0).UnixNano() / 1e6
	if minTime == 0 || options.BaseTime < minTime || options.BaseTime > time.Now().UnixNano()/1e6 {
		panic("BaseTime error.")
	}

	if options.SeqBitLength+options.WorkerIdBitLength > 22 {
		panic("error：WorkerIdBitLength + SeqBitLength <= 22")
	}

	maxWorkerIdNumber := uint16(math.Pow(float64(2), float64(options.WorkerIdBitLength))) - 1
	if options.WorkerId > maxWorkerIdNumber {
		panic("WorkerId error. (range:[1, " + string(maxWorkerIdNumber) + "]")
	}

	if options.SeqBitLength < 2 || options.SeqBitLength > 21 {
		panic("SeqBitLength error. (range:[2, 21])")
	}

	maxSeqNumber := uint32(math.Pow(2, float64(options.SeqBitLength))) - 1
	if options.MaxSeqNumber > maxSeqNumber {
		panic("MaxSeqNumber error. (range:[1, " + string(maxSeqNumber) + "]")
	}

	if options.MinSeqNumber > maxSeqNumber {
		panic("MinSeqNumber error. (range:[1, " + string(maxSeqNumber) + "]")
	}

	var snowWorker contract.ISnowWorker

	switch options.Method {
	case 1:
		snowWorker = core.NewSnowWorkerM1(options)
	case 2:
		snowWorker = core.NewSnowWorkerM2(options)
	default:
		snowWorker = core.NewSnowWorkerM1(options)
	}

	if options.Method == 1 {
		time.Sleep(time.Duration(500) * time.Microsecond)
	}
	return &DefaultIdGenerator{
		Options:    options,
		SnowWorker: snowWorker,
	}
}

func (dig DefaultIdGenerator) NewLong() uint64 {
	return dig.SnowWorker.NextId()
}
