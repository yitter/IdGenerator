/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package gen

import (
	"sync"
	"yitidgen/contract"
)

var ins *YitIdHelper
var once sync.Once

type YitIdHelper struct {
	idGenInstance interface {
		NewLong() uint64
	}
}

func GetIns() *YitIdHelper {
	once.Do(func() {
		ins = &YitIdHelper{}
	})
	return ins
}

func (yih *YitIdHelper) GetIdGenInstance() interface{} {
	return yih.idGenInstance
}

func (yih *YitIdHelper) SetIdGenerator(options *contract.IdGeneratorOptions) {
	yih.idGenInstance = NewDefaultIdGenerator(options)
}

func (yih *YitIdHelper) NextId() uint64 {
	once.Do(func() {
		if yih.idGenInstance == nil {
			options := contract.NewIdGeneratorOptions(1)
			yih.idGenInstance = NewDefaultIdGenerator(options)
		}
	})

	return yih.idGenInstance.NewLong()
}
