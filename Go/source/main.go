package main

import (
	"C"
	"fmt"
	"time"
	"yitidgen/contract"
	"yitidgen/idgen"
	"yitidgen/regworkerid"
)

///export SetOptions
func SetOptions(workerId uint16) {
	var options = contract.NewIdGeneratorOptions(workerId)
	idgen.SetIdGenerator(options)
}

///export NextId
func NextId() uint64 {
	return idgen.NextId()
}

// 注册一个新的WorkerId
//export RegisterWorkerId
func RegisterWorkerId(ip *C.char, port int, password *C.char, maxWorkerId int) int {
	return regworkerid.RegisterWorkerId(C.GoString(ip), port, C.GoString(password), maxWorkerId)
}

// 注销WorkerId
//export UnRegisterWorkerId
func UnRegisterWorkerId() {
	regworkerid.UnRegisterWorkerId()
}

// 检查本地WorkerId是否有效
//export ValidateLocalWorkerId
func ValidateLocalWorkerId(workerId int) bool {
	return regworkerid.ValidateLocalWorkerId(workerId)
}

func main() {
	// 方法一：直接采用默认方法生成一个Id
	fmt.Println("生成的Id:", idgen.NextId())

	fmt.Println("注册的WorkerId:", regworkerid.RegisterWorkerId("localhost", 6379, "", 4))

	return
	// 方法二：自定义参数
	var options = contract.NewIdGeneratorOptions(1)
	options.WorkerIdBitLength = 6
	options.SeqBitLength = 6
	options.TopOverCostCount = 2000
	options.BaseTime = time.Date(2020, 2, 20, 2, 20, 2, 20, time.UTC).UnixNano() / 1e6
	idgen.SetIdGenerator(options)

	var genCount = 50000

	for {
		var begin = time.Now().UnixNano() / 1e6
		for i := 0; i < genCount; i++ {
			idgen.NextId()
		}
		var end = time.Now().UnixNano() / 1e6

		fmt.Println(end - begin)
		time.Sleep(time.Duration(1000) * time.Millisecond)
	}

}

// go build -o target\yitidgengo.dll -buildmode=c-shared main.go

//var yid = idgen.YitIdHelper{}
//yid.SetIdGenerator(options)
//fmt.Println(yid.NextId())
