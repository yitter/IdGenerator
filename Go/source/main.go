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
func NextId() int64 {
	return idgen.NextId()
}

// RegisterOne 注册一个 WorkerId，会先注销所有本机已注册的记录
// address: Redis连接地址，单机模式示例：127.0.0.1:6379，哨兵/集群模式示例：127.0.0.1:26380,127.0.0.1:26381,127.0.0.1:26382
// password: Redis连接密码
// db: Redis指定存储库，示例：1
// sentinelMasterName: Redis 哨兵模式下的服务名称，示例：mymaster
// maxWorkerId: WorkerId 最大值，示例：63
//export RegisterOne
func RegisterOne(address *C.char, password *C.char, db int, sentinelMasterName *C.char, maxWorkerId int32) int32 {
	return regworkerid.RegisterOne(regworkerid.RegisterConf{
		Address:     C.GoString(address),
		Password:    C.GoString(password),
		DB:          db,
		MasterName:  C.GoString(sentinelMasterName),
		MaxWorkerId: maxWorkerId,
	})
}

// RegisterMany 注册多个 WorkerId，会先注销所有本机已注册的记录
// address: Redis连接地址，单机模式示例：127.0.0.1:6379，哨兵/集群模式示例：127.0.0.1:26380,127.0.0.1:26381,127.0.0.1:26382
// password: Redis连接密码
// db: Redis指定存储库，示例：1
// sentinelMasterName: Redis 哨兵模式下的服务名称，示例：mymaster
// maxWorkerId: WorkerId 最大值，示例：63
// totalCount: 获取N个WorkerId，示例：5
//export RegisterMany
func RegisterMany(address *C.char, password *C.char, db int, sentinelMasterName *C.char, maxWorkerId int32, totalCount int32) []int32 {
	return regworkerid.RegisterMany(regworkerid.RegisterConf{
		Address:     C.GoString(address),
		Password:    C.GoString(password),
		DB:          db,
		MasterName:  C.GoString(sentinelMasterName),
		MaxWorkerId: maxWorkerId,
		TotalCount:  totalCount,
	})
}

// UnRegister 注销本机已注册的 WorkerId
//export UnRegister
func UnRegister() {
	regworkerid.UnRegister()
}

// Validate 检查本地WorkerId是否有效（0-有效，其它-无效）
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
				id := idgen.NextId()
				fmt.Println(id)
			}
			var end = time.Now().UnixNano() / 1e3

			fmt.Println(end - begin)
			time.Sleep(time.Duration(1000) * time.Millisecond)
		}
	} else {

		//// Redis单机模式下，获取多个WorkerId
		//workerIdList := regworkerid.RegisterMany(regworkerid.RegisterConf{
		//	Address:     "127.0.0.1:6379",
		//	Password:    "",
		//	DB:          0,
		//	MaxWorkerId: 4,
		//	TotalCount:  3,
		//})
		//for _, value := range workerIdList {
		//	fmt.Println("RegisterMany--注册的WorkerId:", value)
		//}

		// Redis单机模式下，获取一个WorkerId
		var workerId = regworkerid.RegisterOne(regworkerid.RegisterConf{
			Address:     "127.0.0.1:6379",
			Password:    "",
			DB:          0,
			MaxWorkerId: 4,
		})
		fmt.Println("RegisterOne--注册的WorkerId:", workerId)

		//// Redis哨兵模式下，获取一个WorkerId
		//var workerId = regworkerid.RegisterOne(regworkerid.RegisterConf{
		//	//Address:     "127.0.0.1:26380,127.0.0.1:26381,127.0.0.1:26382",
		//	Password:    "",
		//	DB:          0,
		//	MasterName:  "mymaster", // 哨兵模式下的服务名称
		//	MaxWorkerId: 4,
		//})
		//fmt.Println("RegisterOne--注册的WorkerId:", workerId)

		fmt.Println("end")
		time.Sleep(time.Duration(300) * time.Second)
	}
}

// windows:
// go build -o ./target/yitidgengo.dll -buildmode=c-shared main.go

// linux init: go install -buildmode=shared -linkshared std
// go build -o ./target/yitidgengo.so -buildmode=c-shared main.go
