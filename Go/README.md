# idgenerator

## Go环境

1.SDK，go 1.16

2. 启用 Go-Modules
```
go env -w GO111MODULE=on
go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
```


## Go代码示例
```
var yid = idgen.YitIdHelper{}
fmt.Println(yid.NextId())

// 方法二：自定义参数
var options = contract.NewIdGeneratorOptions(1)
//options.WorkerIdBitLength = 6
//options.SeqBitLength = 6
//options.TopOverCostCount = 2000
//options.BaseTime = time.Date(2020, 2, 20, 2, 20, 2, 20, time.UTC).UnixNano() / 1e6
yid.SetIdGenerator(options)

```

