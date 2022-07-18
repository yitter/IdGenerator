package main

import (
	"C"
	"fmt"
	"time"

	"github.com/yitter/idgenerator-go/regworkerid"
)

func main() {
	// ip := "localhost"
	ipChar := C.CString("192.168.20.41")
	passChar := C.CString("")

	workerIdList := RegisterMany(ipChar, 6379, passChar, 4, 3, 0)
	for _, value := range workerIdList {
		fmt.Println("注册的WorkerId:", value)
	}

	id := RegisterOne(ipChar, 6379, passChar, 4, 0)
	fmt.Println("注册的WorkerId:", id)

	// C.free(unsafe.Pointer(ipChar))
	// C.free(unsafe.Pointer(passChar))

	// var workerId = regworkerid.RegisterOne(ip, 6379, "", 4)
	// fmt.Println("注册的WorkerId:", workerId)

	fmt.Println("end")
	time.Sleep(time.Duration(300) * time.Second)
}

//export RegisterOne
// 注册一个 WorkerId，会先注销所有本机已注册的记录
func RegisterOne(ip *C.char, port int32, password *C.char, maxWorkerId int32, database int) int32 {
	return regworkerid.RegisterOne(C.GoString(ip), port, C.GoString(password), maxWorkerId, database)
}

// RegisterMany
// 注册多个 WorkerId，会先注销所有本机已注册的记录
func RegisterMany(ip *C.char, port int32, password *C.char, maxWorkerId, totalCount int32, database int) []int32 {
	// return (*C.int)(unsafe.Pointer(&values))
	//return regworkerid.RegisterMany(ip, port, password, maxWorkerId, totalCount, database)
	return regworkerid.RegisterMany(C.GoString(ip), port, C.GoString(password), maxWorkerId, totalCount, database)
}

//export UnRegister
// 注销本机已注册的 WorkerId
func UnRegister() {
	regworkerid.UnRegister()
}

//export Validate
// 检查本地WorkerId是否有效（0-有效，其它-无效）
func Validate(workerId int32) int32 {
	return regworkerid.Validate(workerId)
}

// To Build a dll/so：

// windows:
// go build -o ./target/yitidgengo.dll -buildmode=c-shared main.go
// // go build -o ./target/yitidgengo.dll -buildmode=c-shared main.go reg.go

// linux init: go install -buildmode=shared -linkshared std
// go build -o ./target/yitidgengo.so -buildmode=c-shared main.go
// go build -o ./target/yitidgengo.so -buildmode=c-shared main.go reg.go

// https://books.studygolang.com/advanced-go-programming-book/ch2-cgo/ch2-09-static-shared-lib.html
