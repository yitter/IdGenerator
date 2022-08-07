from .Options import IdGeneratorOptions
from .SnowFlakeM1 import SnowFlakeM1

class DefaultIdGenerator():

	def SetIdGernerator(self, options:IdGeneratorOptions) :
		if options.BaseTime < 100000 :
			raise ValueError ("BaseTime error.")
		
		self.SnowFlake= SnowFlakeM1(options)

	def NextId(self) -> int:
		"""
        获取新的UUID
        """
		return self.SnowFlake.NextId()

