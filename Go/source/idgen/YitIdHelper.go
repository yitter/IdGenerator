/*
 * 版权属于：yitter(yitter@126.com)
 * 代码编辑：guoyahao
 * 代码修订：yitter
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package idgen

import (
	"sync"
)

//var yitIdHelper *YitIdHelper
//var once sync.Once

var idGenerator *DefaultIdGenerator
var singletonMutex sync.Mutex

type YitIdHelper struct {
	idGenInstance interface {
		NewLong() uint64
	}
}

//
//func GetIns() *YitIdHelper {
//	once.Do(func() {
//		yitIdHelper = &YitIdHelper{}
//	})
//	return yitIdHelper
//}
//
//func (yih *YitIdHelper) GetIdGenInstance() interface{} {
//	return yih.idGenInstance
//}
//
//func (yih *YitIdHelper) SetIdGenerator(options *contract.IdGeneratorOptions) {
//	yih.idGenInstance = NewDefaultIdGenerator(options)
//}
//
//func (yih *YitIdHelper) NextId() uint64 {
//	once.Do(func() {
//		if yih.idGenInstance == nil {
//			options := contract.NewIdGeneratorOptions(1)
//			yih.idGenInstance = NewDefaultIdGenerator(options)
//		}
//	})
//
//	return yih.idGenInstance.NewLong()
//}

func SetIdGenerator(options *IdGeneratorOptions) {
	singletonMutex.Lock()
	idGenerator = NewDefaultIdGenerator(options)
	singletonMutex.Unlock()
}

func NextId() uint64 {
	if idGenerator == nil {
		singletonMutex.Lock()
		if idGenerator == nil {
			options := NewIdGeneratorOptions(1)
			idGenerator = NewDefaultIdGenerator(options)
		}
		singletonMutex.Unlock()
	}

	return idGenerator.NewLong()
}
