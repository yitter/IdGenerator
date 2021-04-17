/*
设计说明：

1.这是SQL Server的一个内置方法，运行此脚本后，将在SQL Server的“可编程性-函数-标量值函数”中增加一个方法 Fn_NextSnowId

2.生成的ID = 时间差 + WokerId + 随机数
	时间差 = 当前时间戳（毫秒单位） - 1582136402000
	WorkerId = {配置值}
	随机数 = 5 至 2^SeqBigLength-1 之间的整数

3.调用方法：
	例如：select dbo.Fn_NextSnowId(rand())

*/
CREATE function dbo.Fn_NextSnowId
(
	@RandomSeed float -- 生成ID的函数，需要有一个随机数，在调用时，传入系统函数 rand() 即可
)
returns bigint
as
begin
	declare @CurrentTime bigint
	declare @TimeTick bigint
	declare @WorkerId int
	declare @WorkerIdBigLength int
	declare @SeqBigLength int

	-- Begin: 以下是需要初始化的参数，请确保 @WorkerIdBigLength 和 @SeqBigLength 的设置值跟其它应用程序相同
	set @WorkerId = 1 -- 最大值 2^@WorkerIdBigLength-1
	set @WorkerIdBigLength = 4 -- @WorkerIdBigLength+@SeqBigLength<23
	set @SeqBigLength = 8	-- 建议不小于6
	-- End

	-- 当前时间戳（毫秒单位）
	set @CurrentTime = CONVERT(BIGINT,DATEDIFF(MI,'1970-01-01 00:00:00.000', GETUTCDATE())) * 60000 + DATEPART(S,GETUTCDATE()) * 1000 + DATEPART(MS, GETUTCDATE())

	-- 用当前时间戳减去基础时间，得出ID的时间差部分
	set @TimeTick=@CurrentTime-1582136402000 

	-- 生成ID
	-- 雪花ID的序列数 = 5至2^SeqBigLength-1之间的随机数。 (5 + round((POWER(2, @SeqBigLength)-1) * rand(), 0)
	return @TimeTick * POWER(2, @WorkerIdBigLength + @SeqBigLength) + @WorkerId * POWER(2, @SeqBigLength) + (5 + round((POWER(2, @SeqBigLength)-1) * @RandomSeed, 0))
end
GO


