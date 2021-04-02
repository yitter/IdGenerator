/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
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

	minTime := int64(631123200000) // time.Now().AddDate(-30, 0, 0).UnixNano() / 1e6
	if options.BaseTime < minTime || options.BaseTime > time.Now().UnixNano()/1e6 {
		panic("BaseTime error.")
	}

	if options.SeqBitLength+options.WorkerIdBitLength > 22 {
		panic("error：WorkerIdBitLength + SeqBitLength <= 22")
	}

	maxWorkerIDNumber := uint16(1<<options.WorkerIdBitLength) - 1
	if options.WorkerId > maxWorkerIDNumber {
		panic("WorkerId error. (range:[1, " + strconv.FormatUint(uint64(maxWorkerIDNumber), 10) + "]")
	}

	if options.SeqBitLength < 2 || options.SeqBitLength > 21 {
		panic("SeqBitLength error. (range:[2, 21])")
	}

	maxSeqNumber := uint32(1<<options.SeqBitLength) - 1
	if options.MaxSeqNumber > maxSeqNumber {
		panic("MaxSeqNumber error. (range:[1, " + strconv.FormatUint(uint64(maxSeqNumber), 10) + "]")
	}

	if options.MinSeqNumber > maxSeqNumber {
		panic("MinSeqNumber error. (range:[1, " + strconv.FormatUint(uint64(maxSeqNumber), 10) + "]")
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

func (dig DefaultIdGenerator) NewLong() uint64 {
	return dig.SnowWorker.NextId()
}
