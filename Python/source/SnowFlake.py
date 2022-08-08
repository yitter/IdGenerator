#!/usr/bin/python
# coding=UTF-8

# 组件编号生成器
class SnowFlake(object):
	
	def __init__(self, options):
		self.Options = options
		
	def NextId(self) -> int:
		return 0
