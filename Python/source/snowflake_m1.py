"""
M1生成器
"""

# !/usr/bin/python
# coding=UTF-8

import threading
import time
from .snowflake import SnowFlake
from .options import IdGeneratorOptions


class SnowFlakeM1(SnowFlake):
    """
    M1规则ID生成器配置
    """

    def __init__(self, options: IdGeneratorOptions):
        # 1.base_time
        self.base_time = 1582136402000
        if options.base_time != 0:
            self.base_time = int(options.base_time)

        # 2.worker_id_bit_length
        self.worker_id_bit_length = 6
        if options.worker_id_bit_length != 0:
            self.worker_id_bit_length = int(options.worker_id_bit_length)

        # 3.worker_id
        self.worker_id = options.worker_id

        # 4.seq_bit_length
        self.seq_bit_length = 6
        if options.seq_bit_length != 0:
            self.seq_bit_length = int(options.seq_bit_length)

        # 5.max_seq_number
        self.max_seq_number = int(options.max_seq_number)
        if options.max_seq_number <= 0:
            self.max_seq_number = (1 << self.seq_bit_length) - 1

        # 6.min_seq_number
        self.min_seq_number = int(options.min_seq_number)

        # 7.top_over_cost_count
        self.top_over_cost_count = int(options.top_over_cost_count)

        # 8.Others
        self.__timestamp_shift = self.worker_id_bit_length + self.seq_bit_length
        self.__current_seq_number = self.min_seq_number
        self.__last_time_tick: int = 0
        self.__turn_back_time_tick: int = 0
        self.__turn_back_index: int = 0
        self.__is_over_cost = False
        self.___over_cost_count_in_one_term: int = 0
        self.__id_lock = threading.Lock()

    def __next_over_cost_id(self) -> int:
        current_time_tick = self.__get_current_time_tick()
        if current_time_tick > self.__last_time_tick:
            self.__last_time_tick = current_time_tick
            self.__current_seq_number = self.min_seq_number
            self.__is_over_cost = False
            self.___over_cost_count_in_one_term = 0
            return self.__calc_id(self.__last_time_tick)

        if self.___over_cost_count_in_one_term >= self.top_over_cost_count:
            self.__last_time_tick = self.__get_next_time_tick()
            self.__current_seq_number = self.min_seq_number
            self.__is_over_cost = False
            self.___over_cost_count_in_one_term = 0
            return self.__calc_id(self.__last_time_tick)

        if self.__current_seq_number > self.max_seq_number:
            self.__last_time_tick += 1
            self.__current_seq_number = self.min_seq_number
            self.__is_over_cost = True
            self.___over_cost_count_in_one_term += 1
            return self.__calc_id(self.__last_time_tick)

        return self.__calc_id(self.__last_time_tick)

    def __next_normal_id(self) -> int:
        current_time_tick = self.__get_current_time_tick()
        if current_time_tick < self.__last_time_tick:
            if self.__turn_back_time_tick < 1:
                self.__turn_back_time_tick = self.__last_time_tick - 1
                self.__turn_back_index += 1
                # 每毫秒序列数的前5位是预留位, 0用于手工新值, 1-4是时间回拨次序
                # 支持4次回拨次序（避免回拨重叠导致ID重复）, 可无限次回拨（次序循环使用）。
                if self.__turn_back_index > 4:
                    self.__turn_back_index = 1

            return self.__calc_turn_back_id(self.__turn_back_time_tick)

        # 时间追平时, _TurnBackTimeTick清零
        self.__turn_back_time_tick = min(self.__turn_back_time_tick, 0)

        if current_time_tick > self.__last_time_tick:
            self.__last_time_tick = current_time_tick
            self.__current_seq_number = self.min_seq_number
            return self.__calc_id(self.__last_time_tick)

        if self.__current_seq_number > self.max_seq_number:
            self.__last_time_tick += 1
            self.__current_seq_number = self.min_seq_number
            self.__is_over_cost = True
            self.___over_cost_count_in_one_term = 1
            return self.__calc_id(self.__last_time_tick)

        return self.__calc_id(self.__last_time_tick)

    def __calc_id(self, use_time_tick) -> int:
        self.__current_seq_number += 1
        return (
                       (use_time_tick << self.__timestamp_shift) +
                       (self.worker_id << self.seq_bit_length) +
                       self.__current_seq_number
               ) % int(1e64)

    def __calc_turn_back_id(self, use_time_tick) -> int:
        self.__turn_back_time_tick -= 1
        return (
                       (use_time_tick << self.__timestamp_shift) +
                       (self.worker_id << self.seq_bit_length) +
                       self.__turn_back_index
               ) % int(1e64)

    def __get_current_time_tick(self) -> int:
        return int((time.time_ns() / 1e6) - self.base_time)

    def __get_next_time_tick(self) -> int:
        temp_time_ticker = self.__get_current_time_tick()
        while temp_time_ticker <= self.__last_time_tick:
            # 0.001 = 1 mili sec
            time.sleep(0.001)
            temp_time_ticker = self.__get_current_time_tick()
        return temp_time_ticker

    def next_id(self) -> int:
        with self.__id_lock:
            if self.__is_over_cost:
                nextid = self.__next_over_cost_id()
            else:
                nextid = self.__next_normal_id()
            return nextid
