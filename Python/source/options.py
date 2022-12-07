"""
生成器IdGenerator配置选项
"""

# !/usr/bin/python
# coding=UTF-8


class IdGeneratorOptions:
    """
    ID生成器配置
    - worker_id 全局唯一id, 区分不同uuid生成器实例
    - worker_id_bit_length 生成的uuid中worker_id占用的位数
    - seq_bit_length 生成的uuid中序列号占用的位数
    """

    def __init__(self, worker_id=0, worker_id_bit_length=6, seq_bit_length=6):

        # 雪花计算方法,（1-漂移算法|2-传统算法）, 默认1。目前只实现了1。
        self.method = 1

        # 基础时间（ms单位）, 不能超过当前系统时间
        self.base_time = 1582136402000

        # 机器码, 必须由外部设定, 最大值 2^worker_id_bit_length-1
        self.worker_id = worker_id

        # 机器码位长, 默认值6, 取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
        self.worker_id_bit_length = worker_id_bit_length

        # 序列数位长, 默认值6, 取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
        self.seq_bit_length = seq_bit_length

        # 最大序列数（含）, 设置范围 [max_seq_number, 2^seq_bit_length-1]
        # 默认值0, 表示最大序列数取最大值（2^seq_bit_length-1]）
        self.max_seq_number = 0

        # 最小序列数（含）, 默认值5, 取值范围 [5, max_seq_number], 每毫秒的前5个序列数对应编号0-4是保留位
        # 其中1-4是时间回拨相应预留位, 0是手工新值预留位
        self.min_seq_number = 5

        # 最大漂移次数（含）, 默认2000, 推荐范围500-10000（与计算能力有关）
        self.top_over_cost_count = 2000
