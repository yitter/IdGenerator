# IdGen

#### 介绍
用一种全新的雪花漂移算法，让ID更短、生成速度更快。
核心在于缩短ID长度的同时，还能保持每毫秒并发处理量（50W/0.1s），且能保持伸缩能力。

## 功能说明

1.ID更短，是传统算法的几倍，用50年都不会超过js（Number）的最大值。（默认配置WorkerId是6bit，自增数是6bit）

2.生成速度更快，0.1秒可生成50万个。（i7笔记本，默认算法配置6bit+6bit）

3.支持时间回拨处理。比如Linux服务器时间回拨1秒，本算法能自动适应生成临界时间的唯一ID。

4.支持手工插入新ID。当业务需要在历史时间生成新ID时，用本算法的预留位每秒能生成5000个。

5.漂移时对外异步发通知事件。让调用方确切知道算法漂移记录，Log并发调用量。

6.不依赖任何外部缓存和数据库。（但WorkerId必须由外部指定）

7.目前是C#版，很快会出java、php等版本。


## 文件说明

1.SnowWorkerM1.cs 是雪花漂移算法。

2.SnowWorkerM2.cs 是传统雪花算法。


## 效果

1.js Number 类型最大数值：9007199254740992，本算法在保持并发性能（5W+/0.01s）和最大64个WorkerId（6bit）的同时，能用70年才到 js Number Max 值。

2.增加WorkerId位数到8bit（128节点）时，15年达到 js Number Max 值。

3.极致性能：500W/1s。

4.所有测试数据均基于8代低压i7计算。


## 适用范围

1.小型、中型、大型需要全局唯一Id（不用Guid）的项目。

2.分布式项目。

3.不想将Long型转String给前端用的项目。（如果前端支持bigint，则不用考虑此项）


##  大型分布式环境使用建议

1.可扩大 WorkerIdBitLength 到最多20位，支持 1,048,576 个节点，且不影响上述并发性能（50W/0.1s）。

2.采用中心化 IdGenerator 集群，给节点生成可用 Id 列表，存入 Redis 队列供节点消费。此时64个中心化节点数足够大型互联网项目使用。

## 使用方法

#### 雪花漂移算法
```
var options = new IdGeneratorOptions()
{
	// 设置WorkerId，默认最大2^16-1
	WorkerId = 1
};

var newId = new YitIdGenerator(options).NewLong();
```

#### 传统雪花算法
```
var options = new IdGeneratorOptions()
{
	Method = 2,  // 默认1
	WorkerId = 1
};

var newId = new YitIdGenerator(options).NewLong();
```

## options说明
options参数（Method、StartTime除外）只支持漂移算法，不支持传统雪花算法。

```
public class IdGeneratorOptions
{
    /// <summary>
    /// 雪花计算方法
    /// （1|2）
    /// </summary>
    public short Method { get; set; } = 1;

    /// <summary>
    /// 开始时间（UTC格式）
    /// 不能超过当前系统时间
    /// </summary>
    public DateTime StartTime { get; set; } = DateTime.MinValue;

    /// <summary>
    /// 机器码
    /// 与 WorkerIdBitLength 有关系
    /// </summary>
    public ushort WorkerId { get; set; } = 0;

    /// <summary>
    /// 机器码位长
    /// 范围：2-21（要求：序列数位长+机器码位长不超过22）。
    /// 建议范围：6-12。
    /// </summary>
    public byte WorkerIdBitLength { get; set; } = 6;//10;

    /// <summary>
    /// 序列数位长
    /// 范围：2-21（要求：序列数位长+机器码位长不超过22）。
    /// 建议范围：6-14。
    /// </summary>
    public byte SeqBitLength { get; set; } = 6;//10;

    /// <summary>
    /// 最大序列数（含）
    /// （由SeqBitLength计算的最大值）
    /// </summary>
    public int MaxSeqNumber { get; set; } = 0;

    /// <summary>
    /// 最小序列数（含）
    /// 默认11，不小于5，不大于MaxSeqNumber-2
    /// </summary>
    public ushort MinSeqNumber { get; set; } = 11;

    /// <summary>
    /// 最大漂移次数（含），
    /// 默认2000，推荐范围500-10000（与计算能力有关）
    /// </summary>
    public int TopOverCostCount { get; set; } = 2000;
```

## 技术支持

QQ群：646049993

