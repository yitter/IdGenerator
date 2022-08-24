
"""
雪花算法生成器接口声明
"""

# !/usr/bin/python
# coding=UTF-8


class SnowFlake():

    def __init__(self, options):
        self.options = options

    def next_id(self) -> int:
        """
        获取新的UUID
        """
        
        return 0
