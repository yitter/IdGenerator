
## 运行环境

.NET Standard 2.0+

## 引用nuget包
```
<PackageReference Include="Yitter.IdGenerator" Version="1.0.*" />
```

## 调用示例
```
// 全局初始化设置WorkerId，默认最大2^16-1。（初始化过程全局只需一次，且必须最先设置）
var options = new IdGeneratorOptions(){ WorkerId = 1};
YitIdHelper.SetIdGenerator(options);

// 初始化以后，就可以在需要的地方调用方法生成ID。
var newId = YitIdHelper.NextId();

```
如果基于DI框架集成，可以参考 YitIdHelper 去管理 IdGenerator 对象，必须使用**单例**模式。

## options 默认值及说明

参考源码：/Contract/IdGeneratorOptions.cs

## 事件说明

1.IIdGenerator.GenIdActionAsync 是一个可以向外部系统异步发送ID生成消息的事件，它包含的消息类型有"漂移开始、漂移结束、时间回拨"，具体参考 Yitter.IdGenTest 的 Program.cs 启动代码。不过订阅ID异步通知会有细微的性能损失。

2.你可在外部系统的异步（async标记）方法中调用本算法，同步调用同样没问题。


