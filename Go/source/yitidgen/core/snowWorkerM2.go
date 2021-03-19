/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package core

import (
	"../contract"
	"fmt"
	"strconv"
	"sync/atomic"
)

type SnowWorkerM2 struct {
	*SnowWorkerM1
}

func NewSnowWorkerM2(options *contract.IdGeneratorOptions) contract.ISnowWorker {
	return &SnowWorkerM2{
		NewSnowWorkerM1(options).(*SnowWorkerM1),
	}
}

func (m2 SnowWorkerM2) NextId() uint64 {
	currentTimeTick := m2.GetCurrentTimeTick()
	if m2._LastTimeTick == currentTimeTick {
		atomic.AddUint32(&m2._CurrentSeqNumber, 1)
		if m2._CurrentSeqNumber > m2.MaxSeqNumber {
			atomic.StoreUint32(&m2._CurrentSeqNumber, uint32(m2.MinSeqNumber))
			currentTimeTick = m2.GetNextTimeTick()
		}
	} else {
		atomic.StoreUint32(&m2._CurrentSeqNumber, uint32(m2.MinSeqNumber))
	}
	if currentTimeTick < m2._LastTimeTick {
		fmt.Println("Time error for {0} milliseconds", strconv.FormatInt(m2._LastTimeTick-currentTimeTick, 10))
	}
	m2._LastTimeTick = currentTimeTick
	result := uint64((currentTimeTick << m2._TimestampShift)) + uint64(m2.WorkerId<<m2.SeqBitLength) + uint64(m2._CurrentSeqNumber)
	return result
}
