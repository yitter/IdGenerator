import time
import traceback
from IdGeneratorOptions  import IdGeneratorOptions
from SnowFlake import SnowFlake
from SnowFlakeM1 import SnowFlakeM1

class DefaultIdGenerator(object):
	
	def SetIdGernerator(self, options) :
		if options.BaseTime < 100000 :
			raise ValueError ("BaseTime error.")
		
		self.SnowFlake= SnowFlakeM1(options)

	def NextId(self):
		return self.SnowFlake.NextId()

if __name__ == '__main__':
	try:
		options = IdGeneratorOptions(23)
		options.BaseTime = 1231111111
		idgen = DefaultIdGenerator()
		idgen.SetIdGernerator(options)
		
		print (idgen.NextId())
		print (options.__dict__)
		
	except ValueError  as e:
		print(e)


