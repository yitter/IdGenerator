package idgen

import (
	"../contract"
	"sync"
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
