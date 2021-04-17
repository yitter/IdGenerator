#  ❄ idgenerator-SQL

## 介绍

这里是在不同数据库中生成雪花ID的SQL脚本。

## SQL Server

设计说明：

```
1.这是SQL Server的一个内置方法，运行此脚本后，将在SQL Server的“可编程性-函数-标量值函数”中增加一个方法 Fn_NextSnowId

2.生成的ID = 时间差 + WokerId + 随机数
	时间差 = 当前时间戳（毫秒单位） - 1582136402000
	WorkerId = {配置值}
	随机数 = 5 至 2^SeqBigLength-1 之间的整数

3.调用方法：
	例如：select dbo.Fn_NextSnowId(rand())
	说明：必须带 dbo. 前缀
	
4.自动赋值：
	如果主键设置为雪花ID类型（bigint），可以将该主键的 “默认值或绑定” 设置为 ([dbo].[Fn_NextSnowId](rand()))，以便在Insert记录时，能给主键自动赋值，而无需外部传入。
```

在执行函数之前，必须设置好以下3个参数：
```
set @WorkerId = 1 -- 最大值 2^@WorkerIdBigLength-1
set @WorkerIdBigLength = 4 -- 规则约束：@WorkerIdBigLength+@SeqBigLength<23
set @SeqBigLength = 8 -- 建议不小于6，在当前SQL版本中，@SeqBigLength 决定随机数的最大值（未采用自增数，这需要数据表记录Seq值）
```

## 其它思路

除了用SQL Server方法生成雪花ID之外，还可让SQL Server调用外部dll生成ID。参考：https://www.cnblogs.com/woxpp/p/3990277.html

