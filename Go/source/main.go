package main

import (
	"C"
	"fmt"
	"time"
	"yitidgen/idgen"
	"yitidgen/regworkerid"
)

//export SetOptions
func SetOptions(workerId uint16) {
	var options = idgen.NewIdGeneratorOptions(workerId)
	idgen.SetIdGenerator(options)
}

//export NextId
func NextId() uint64 {
	return idgen.NextId()
}

// 注册一个新的WorkerId
//export RegisterWorkerId
func RegisterWorkerId(ip *C.char, port int, password *C.char, maxWorkerId int) int {
	return int(regworkerid.RegisterWorkerId(C.GoString(ip), port, C.GoString(password), maxWorkerId))
}

// 注销WorkerId
//export UnRegisterWorkerId
func UnRegisterWorkerId() {
	regworkerid.UnRegisterWorkerId()
}

// 检查本地WorkerId是否有效（0-有效，其它-无效）
//export ValidateLocalWorkerId
func ValidateLocalWorkerId(workerId int) int {
	return regworkerid.ValidateLocalWorkerId(workerId)
}

func main() {
	// 方法一：直接采用默认方法生成一个Id
	fmt.Println("生成的Id:", idgen.NextId())

	fmt.Println("注册的WorkerId:", regworkerid.RegisterWorkerId("localhost", 6379, "", 4))

	// 方法二：自定义参数
	var options = idgen.NewIdGeneratorOptions(1)
	options.WorkerIdBitLength = 6
	options.SeqBitLength = 6
	options.BaseTime = time.Date(2020, 2, 20, 2, 20, 2, 20, time.UTC).UnixNano() / 1e6
	idgen.SetIdGenerator(options)

	var genCount = 50000

	for {
		var begin = time.Now().UnixNano() / 1e3
		for i := 0; i < genCount; i++ {
			idgen.NextId()
		}
		var end = time.Now().UnixNano() / 1e3

		fmt.Println(end - begin)
		time.Sleep(time.Duration(1000) * time.Millisecond)
	}

}

// go build -o ./target/yitidgengo.dll -buildmode=c-shared main.go
// go install -buildmode=shared -linkshared std
// go build -o ./target/yitidgengo.so -buildmode=c-shared main.go
