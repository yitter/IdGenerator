#  ❄ idgenerator-Go

## 介绍

项目更多介绍参照：https://github.com/yitter/idgenerator

## 引用全局库
```shell
go get -u -v github.com/yitter/idgenerator-go
```

## 调用示例（Go）

第1步，**全局** 初始化（应用程序启动时执行一次）：
```go
// 创建 IdGeneratorOptions 对象，可在构造函数中输入 WorkerId：
var options = idgen.NewIdGeneratorOptions(Your_Unique_Worker_Id)
// options.WorkerIdBitLength = 10  // 默认值6，限定 WorkerId 最大值为2^6-1，即默认最多支持64个节点。
// options.SeqBitLength = 6; // 默认值6，限制每毫秒生成的ID个数。若生成速度超过5万个/秒，建议加大 SeqBitLength 到 10。
// options.BaseTime = Your_Base_Time // 如果要兼容老系统的雪花算法，此处应设置为老系统的BaseTime。
// ...... 其它参数参考 IdGeneratorOptions 定义。

// 保存参数（务必调用，否则参数设置不生效）：
idgen.SetIdGenerator(options)

// 以上过程只需全局一次，且应在生成ID之前完成。
```

第2步，生成ID：
```go
// 初始化后，在任何需要生成ID的地方，调用以下方法：
var newId = idgen.NextId()
```

## 关于Go环境

1.SDK，go1.16+

2.启用 Go-Modules
```shell
go env -w GO111MODULE=on

# Next *ONLY* for China-Users:
go env -w GOPROXY=https://goproxy.cn,https://goproxy.io,direct
```

## 代码贡献者(按时间顺序)
guoyahao | amuluowin | houseme

