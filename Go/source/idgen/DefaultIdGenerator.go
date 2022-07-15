/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://github.com/yitter/idgenerator
 */

package idgen

import (
	"strconv"
	"time"
)

type DefaultIdGenerator struct {
	Options              *IdGeneratorOptions
	SnowWorker           ISnowWorker
	IdGeneratorException IdGeneratorException
}

func NewDefaultIdGenerator(options *IdGeneratorOptions) *DefaultIdGenerator {
	if options == nil {
		panic("dig.Options error.")
	}

	// 1.BaseTime
	minTime := int64(631123200000) // time.Now().AddDate(-30, 0, 0).UnixNano() / 1e6
	if options.BaseTime < minTime || options.BaseTime > time.Now().UnixNano()/1e6 {
		panic("BaseTime error.")
	}

	// 2.WorkerIdBitLength
	if options.WorkerIdBitLength <= 0 {
		panic("WorkerIdBitLength error.(range:[1, 21])")
	}
	if options.WorkerIdBitLength+options.SeqBitLength > 22 {
		panic("error：WorkerIdBitLength + SeqBitLength <= 22")
	}

	// 3.WorkerId
	maxWorkerIdNumber := uint16(1<<options.WorkerIdBitLength) - 1
	if maxWorkerIdNumber == 0 {
		maxWorkerIdNumber = 63
	}
	if options.WorkerId < 0 || options.WorkerId > maxWorkerIdNumber {
		panic("WorkerId error. (range:[0, " + strconv.FormatUint(uint64(maxWorkerIdNumber), 10) + "]")
	}

	// 4.SeqBitLength
	if options.SeqBitLength < 2 || options.SeqBitLength > 21 {
		panic("SeqBitLength error. (range:[2, 21])")
	}

	// 5.MaxSeqNumber
	maxSeqNumber := uint32(1<<options.SeqBitLength) - 1
	if maxSeqNumber == 0 {
		maxSeqNumber = 63
	}
	if options.MaxSeqNumber < 0 || options.MaxSeqNumber > maxSeqNumber {
		panic("MaxSeqNumber error. (range:[1, " + strconv.FormatUint(uint64(maxSeqNumber), 10) + "]")
	}

	// 6.MinSeqNumber
	if options.MinSeqNumber < 5 || options.MinSeqNumber > maxSeqNumber {
		panic("MinSeqNumber error. (range:[5, " + strconv.FormatUint(uint64(maxSeqNumber), 10) + "]")
	}

	// 7.TopOverCostCount
	if options.TopOverCostCount < 0 || options.TopOverCostCount > 10000 {
		panic("TopOverCostCount error. (range:[0, 10000]")
	}

	var snowWorker ISnowWorker
	switch options.Method {
	case 1:
		snowWorker = NewSnowWorkerM1(options)
	case 2:
		snowWorker = NewSnowWorkerM2(options)
	default:
		snowWorker = NewSnowWorkerM1(options)
	}

	if options.Method == 1 {
		time.Sleep(time.Duration(500) * time.Microsecond)
	}

	return &DefaultIdGenerator{
		Options:    options,
		SnowWorker: snowWorker,
	}
}

func (dig DefaultIdGenerator) NewLong() int64 {
	return dig.SnowWorker.NextId()
}

func (dig DefaultIdGenerator) ExtractTime(id int64) time.Time {
	return time.UnixMilli(id>>(dig.Options.WorkerIdBitLength+dig.Options.SeqBitLength) + dig.Options.BaseTime)
}
