package main

import (
	"fmt"
	"time"
	"yitidgen/contract"
	"yitidgen/idgen"
)

func main() {
	// 方法一：直接采用默认方法生成一个Id
	var yid = idgen.YitIdHelper{}
	fmt.Println(yid.NextId())

	// 方法二：自定义参数
	var options = contract.NewIdGeneratorOptions(1)
	//options.WorkerIdBitLength = 6
	//options.SeqBitLength = 6
	//options.TopOverCostCount = 2000
	//options.BaseTime = time.Date(2020, 2, 20, 2, 20, 2, 20, time.UTC).UnixNano() / 1e6
	yid.SetIdGenerator(options)

	var times = 50000

	for ; ; {
		var begin = time.Now().UnixNano() / 1e6
		for i := 0; i < times; i++ {
			yid.NextId()
		}
		var end = time.Now().UnixNano() / 1e6

		fmt.Println(end - begin)
		time.Sleep(time.Duration(1000) * time.Millisecond)
	}

}
