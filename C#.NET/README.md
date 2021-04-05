
## 运行环境

.NET Standard 2.0+

## 引用nuget包
```
<PackageReference Include="Yitter.IdGenerator" Version="1.0.*" />
```

## 调用示例（C#）

第1步，**全局** 初始化（应用程序启动时执行一次）：
```
// 创建 IdGeneratorOptions 对象，构造函数输入 WorkerId：
var options = new IdGeneratorOptions(1);
// options.WorkerIdBitLength = 10; // WorkerIdBitLength 默认值6，支持的 WorkerId 最大值为2^6-1，若 WorkerId 超过64，可设置更大的 WorkerIdBitLength
// ...... 其它参数设置参考 IdGeneratorOptions 定义，一般来说，只要再设置 WorkerIdBitLength （决定 WorkerId 的最大值）。

// 保存参数（必须的操作，否则以上设置都不能生效）：
YitIdHelper.SetIdGenerator(options);
// 以上初始化过程只需全局一次，且必须在第2步之前设置。
```

第2步，生成ID：
```
// 初始化以后，即可在任何需要生成ID的地方，调用以下方法：
var newId = YitIdHelper.NextId();
```

如果基于DI框架集成，可以参考 YitIdHelper 去管理 IdGenerator 对象，必须使用**单例**模式。


## 事件说明

1.IIdGenerator.GenIdActionAsync 是一个可以向外部系统异步发送ID生成消息的事件，它包含的消息类型有"漂移开始、漂移结束、时间回拨"，具体参考 Yitter.IdGenTest 的 Program.cs 启动代码。不过订阅ID异步通知会有细微的性能损失。

2.你可在外部系统的异步（async标记）方法中调用本算法，同步调用同样没问题。


