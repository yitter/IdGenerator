#!/usr/bin/python
# coding=UTF-8
from SnowFlake  import SnowFlake

# 组件编号生成器
class SnowFlakeM1(SnowFlake):
	
	def __init__(self, options):
		self.Options = options
		
	def NextId(self):
		return self.Options.WorkerId
