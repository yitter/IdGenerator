package main

import (
	"C"
)
import "github.com/yitter/idgenerator-go/regworkerid"

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
