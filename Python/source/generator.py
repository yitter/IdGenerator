"""
雪花算法生成器IdGenerator
"""

# !/usr/bin/python
# coding=UTF-8


from . import options
from . import snowflake_m1


class DefaultIdGenerator:
    """
    ID生成器
    """

    def __init__(self):
        self.snowflake = None

    def set_id_generator(self, option: options.IdGeneratorOptions):
        """
        设置id生成规则信息
        """

        if option.base_time < 100000:
            raise ValueError("base time error.")

        self.snowflake = snowflake_m1.SnowFlakeM1(option)

    def next_id(self) -> int:
        """
        获取新的UUID
        """

        if self.snowflake is None:
            raise ValueError("please set id generator at first.")
        return self.snowflake.next_id()
