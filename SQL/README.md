#  ❄ idgenerator-SQL

## 介绍

这里是在不同数据库中生成雪花ID的脚本。

#### SQL Server

设计说明：

```
1.这是SQL Server的一个内置方法，运行此脚本后，将在SQL Server的“可编程性-函数-标量值函数”中增加一个方法 Fn_NextSnowId

2.生成的ID = 时间差 + WokerId + 随机数
	时间差 = 当前时间戳（毫秒单位） - 1582136402000
	WorkerId = {配置值}
	随机数 = 5 至 2^SeqBigLength-1 之间的整数

3.调用方法：
	例如：select dbo.Fn_NextSnowId(rand())
```

在执行函数之前，必须设置好以下3个参数：
```
	set @WorkerId = 1 -- 最大值 2^@WorkerIdBigLength-1
	set @WorkerIdBigLength = 4 -- @WorkerIdBigLength+@SeqBigLength，不要超过22
	set @SeqBigLength = 8
```
