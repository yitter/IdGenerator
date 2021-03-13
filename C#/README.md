
## 运行环境

.NET Standard 2.0+

## 引用nuget包
```
<PackageReference Include="Yitter.IdGenerator" Version="1.0.2" />
```

## 调用示例
```
// 全局初始化设置WorkerId，默认最大2^16-1。（初始化过程全局只需一次，且必须最先设置）
var options = new IdGeneratorOptions(){ WorkerId = 1};
IdHelper.SetIdGenerator(options);

// 初始化以后，就可以在需要的地方调用方法生成ID。
var newId = IdHelper.NextId();

```
如果基于DI框架集成，可以参考 IdHelper 去管理 IdGenerator 对象，必须使用**单例**模式。

## options说明
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
    public byte WorkerIdBitLength { get; set; } = 6;

    /// <summary>
    /// 序列数位长
    /// 范围：2-21（要求：序列数位长+机器码位长不超过22）。
    /// 建议范围：6-14。
    /// </summary>
    public byte SeqBitLength { get; set; } = 6;

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