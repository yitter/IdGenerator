#!/usr/bin/python
# coding=UTF-8
from .SnowFlake  import SnowFlake
from .Options import IdGeneratorOptions
import threading,time

# 组件编号生成器
class SnowFlakeM1(SnowFlake):
	
	def __init__(self, options:IdGeneratorOptions):
		# 1.BaseTime
		if options.BaseTime != 0:
			self.BaseTime = int(options.BaseTime)
		else:
			self.BaseTime = 1582136402000
		
		# 2.WorkerIdBitLength
		if options.WorkerIdBitLength == 0:
			self.WorkerIdBitLength = 6
		else:
			self.WorkerIdBitLength = int(options.WorkerIdBitLength)
		
		# 3.WorkerId
		self.WorkerId = options.WorkerId

		# 4.SeqBitLength
		if options.SeqBitLength == 0:
			self.SeqBitLength = 6
		else:
			self.SeqBitLength = int(options.SeqBitLength)

		# 5.MaxSeqNumber
		if options.MaxSeqNumber <= 0:
			self.MaxSeqNumber = (1 << self.SeqBitLength) - 1
		else:
			self.MaxSeqNumber = int(options.MaxSeqNumber)

		# 6.MinSeqNumber
		self.MinSeqNumber = int(options.MinSeqNumber)

		# 7.TopOverCostCount
		self.TopOverCostCount = int(options.TopOverCostCount)

		# 8.Others
		self.__TimestampShift = self.WorkerIdBitLength + self.SeqBitLength
		self.__CurrentSeqNumber = self.MinSeqNumber
		self.__LastTimeTick:int = 0
		self.__TurnBackTimeTick:int = 0
		self.__TurnBackIndex:int = 0
		self.__IsOverCost = False 
		self.__OverCostCountInOneTerm:int = 0
		self.__IDLock = threading.Lock()

	def __NextOverCostId(self) -> int:
		CurrentTimeTick = self.__GetCurrentTimeTick()
		if CurrentTimeTick > self.__LastTimeTick:
			self.__LastTimeTick = CurrentTimeTick
			self.__CurrentSeqNumber = self.MinSeqNumber
			self.__IsOverCost = False
			self.__OverCostCountInOneTerm = 0
			return self.__CalcId(self.__LastTimeTick)
		
		if self.__OverCostCountInOneTerm >= self.TopOverCostCount:
			self.__LastTimeTick = self.__GetNextTimeTick()
			self.__CurrentSeqNumber = self.MinSeqNumber
			self.__IsOverCost = False
			self.__OverCostCountInOneTerm = 0
			return self.__CalcId(self.__LastTimeTick)
		
		if self.__CurrentSeqNumber > self.MaxSeqNumber:
			self.__LastTimeTick+=1
			self.__CurrentSeqNumber = self.MinSeqNumber
			self.__IsOverCost = True
			self.__OverCostCountInOneTerm+=1
			return self.__CalcId(self.__LastTimeTick)

		return self.__CalcId(self.__LastTimeTick)
		
	def __NextNormalId(self) -> int:
		CurrentTimeTick = self.__GetCurrentTimeTick()
		if CurrentTimeTick < self.__LastTimeTick:
			if self.__TurnBackTimeTick < 1:
				self.__TurnBackTimeTick = self.__LastTimeTick - 1
				self.__TurnBackIndex+=1
				# 每毫秒序列数的前5位是预留位，0用于手工新值，1-4是时间回拨次序
				# 支持4次回拨次序（避免回拨重叠导致ID重复），可无限次回拨（次序循环使用）。
				if self.__TurnBackIndex > 4:
					self.__TurnBackIndex = 1

			return self.__CalcTurnBackId(self.__TurnBackTimeTick)

		# 时间追平时，_TurnBackTimeTick清零
		if self.__TurnBackTimeTick > 0:
			self.__TurnBackTimeTick = 0

		if CurrentTimeTick > self.__LastTimeTick:
			self.__LastTimeTick = CurrentTimeTick
			self.__CurrentSeqNumber = self.MinSeqNumber
			return self.__CalcId(self.__LastTimeTick)

		if self.__CurrentSeqNumber > self.MaxSeqNumber:
			self.__LastTimeTick+=1
			self.__CurrentSeqNumber = self.MinSeqNumber
			self.__IsOverCost = True
			self.__OverCostCountInOneTerm = 1
			return self.__CalcId(self.__LastTimeTick)

		return self.__CalcId(self.__LastTimeTick)

	def __CalcId(self,useTimeTick) -> int:
		self.__CurrentSeqNumber+=1
		return ((useTimeTick<<self.__TimestampShift) + (self.WorkerId<<self.SeqBitLength) + self.__CurrentSeqNumber) % int(1e64)

	def __CalcTurnBackId(self,useTimeTick) -> int:
		self.__TurnBackTimeTick-=1
		return ((useTimeTick<<self.__TimestampShift) + (self.WorkerId<<self.SeqBitLength) + self.__TurnBackIndex) % int(1e64)

	def __GetCurrentTimeTick(self) -> int:
		return int((time.time_ns() / 1e6) - self.BaseTime)

	def __GetNextTimeTick(self) -> int:
		TempTimeTicker = self.__GetCurrentTimeTick()
		while TempTimeTicker <= self.__LastTimeTick:
			# 0.001 = 1 mili sec
			time.sleep(0.001)
			TempTimeTicker = self.__GetCurrentTimeTick()
		return TempTimeTicker

	def NextId(self) -> int:
		self.__IDLock.acquire()
		if self.__IsOverCost:
			id = self.__NextOverCostId()
		else:
			id = self.__NextNormalId()
		self.__IDLock.release()
		return id
