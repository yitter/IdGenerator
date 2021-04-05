package main

import (
	"C"
	"fmt"
	"time"
	"unsafe"
	"yitidgen/idgen"
	"yitidgen/regworkerid"
)

//export SetOptions
func SetOptions(workerId uint16) {
	var options = idgen.NewIdGeneratorOptions(workerId)
	idgen.SetIdGenerator(options)
}

//export NextId
func NextId() int64 {
	return idgen.NextId()
}

// 注册一个 WorkerId，会先注销所有本机已注册的记录
//export RegisterOne
func RegisterOne(ip *C.char, port int32, password *C.char, maxWorkerId int32) int32 {
	return regworkerid.RegisterOne(C.GoString(ip), port, C.GoString(password), maxWorkerId)
}

// 注册多个 WorkerId，会先注销所有本机已注册的记录
//export RegisterMany
func RegisterMany(ip *C.char, port int32, password *C.char, maxWorkerId int32, totalCount int32) *C.int {
	values := regworkerid.RegisterMany(C.GoString(ip), port, C.GoString(password), maxWorkerId, totalCount)
	return (*C.int)(unsafe.Pointer(&values))
}

// 注销本机已注册的 WorkerId
//export UnRegister
func UnRegister() {
	regworkerid.UnRegister()
}

// 检查本地WorkerId是否有效（0-有效，其它-无效）
//export Validate
func Validate(workerId int32) int32 {
	return regworkerid.Validate(workerId)
}

func main() {
	const testGenId = true // 测试设置

	if testGenId {
		// 自定义参数
		var options = idgen.NewIdGeneratorOptions(1)
		options.WorkerIdBitLength = 6
		options.SeqBitLength = 6
		options.BaseTime = time.Date(2020, 2, 20, 2, 20, 2, 20, time.UTC).UnixNano() / 1e6
		idgen.SetIdGenerator(options)

		var genCount = 50000
		for {
			var begin = time.Now().UnixNano() / 1e3
			for i := 0; i < genCount; i++ {
				// 生成ID
				idgen.NextId()
			}
			var end = time.Now().UnixNano() / 1e3

			fmt.Println(end - begin)
			time.Sleep(time.Duration(1000) * time.Millisecond)
		}
	} else {
		workerIdList := regworkerid.RegisterMany("localhost", 6379, "", 4, 3)
		for _, value := range workerIdList {
			fmt.Println("注册的WorkerId:", value)
		}

		//var workerId = regworkerid.RegisterOne("localhost", 6379, "", 4)
		//fmt.Println("注册的WorkerId:", workerId)

		fmt.Println("end")
		time.Sleep(time.Duration(300) * time.Second)
	}
}

// windows: go build -o ./target/yitidgengo.dll -buildmode=c-shared main.go
//// go install -buildmode=shared -linkshared std
//linux: go build -o ./target/yitidgengo.so -buildmode=c-shared main.go
