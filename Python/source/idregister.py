"""
worker id generator
"""

# !/usr/bin/python
# coding=UTF-8


from threading import Thread
import time
import logging
import redis


class Register:
    """
    redis封装
    - host 代表redis ip
    - port 代表redis端口
    - max_worker_id worker_id的最大值, 默认为100
    - password redis的密码, 默认为空
    """

    def __init__(self, host, port, max_worker_id=100, password=None):
        self.redis_impl = redis.StrictRedis(host=host, port=port, db=0, password=password)
        self.loop_count = 0
        self.max_loop_count = 10
        self.worker_id_expire_time = 15
        self.max_worker_id = max_worker_id
        self.worker_id = -1
        self.is_stop = False

    def get_lock(self, key):
        """
        获取分布式全局锁,并设置过期时间为30秒
        """

        if self.redis_impl.setnx(key, 1):
            self.redis_impl.expire(key, 30)
            return True
        if self.redis_impl.ttl(key) < 0:
            self.redis_impl.expire(key, 30)
        return False

    def stop(self):
        """
        退出注册器的线程
        """

        self.is_stop = True

    def get_worker_id(self):
        """
        获取全局唯一worker_id, 会创建一个线程给worker id续期
        失败返回-1
        """

        self.loop_count = 0

        def extern_life(my_id):
            while 1:
                time.sleep(self.worker_id_expire_time / 3)
                # 是否关闭了
                if self.is_stop:
                    return
                # 更新生命周期
                if self.worker_id != my_id:
                    break
                try:
                    self.redis_impl.expire(
                        f"IdGen:WorkerId:Value:{my_id}",
                        self.worker_id_expire_time)
                except Exception as exe:
                    logging.error(exe)
                continue

        self.worker_id = self.__get_next_worker_id()
        if self.worker_id > -1:
            Thread(target=extern_life, args=[self.worker_id]).start()
        return self.worker_id

    def __get_next_worker_id(self):
        """
        获取全局唯一worker id内部实现
        """

        cur = self.redis_impl.incrby("IdGen:WorkerId:Index", 1)

        def can_reset():
            try:
                reset_value = self.redis_impl.incr("IdGen:WorkerId:Value:Edit")
                return reset_value != 1
            except Exception as ept:
                logging.error(ept)
                return False

        def end_reset():
            try:
                self.redis_impl.set("IdGen:WorkerId:Value:Edit", 0)
            except Exception as ept:
                logging.error(ept)

        def is_available(worker_id: int):
            try:
                rst = self.redis_impl.get(f"IdGen:WorkerId:Value:{worker_id}")
                return rst != "Y"
            except Exception as ept:
                logging.error(ept)
                return False

        if cur > self.max_worker_id:
            if can_reset():
                self.redis_impl.set("IdGen:WorkerId:Index", -1)
                end_reset()
                self.loop_count += 1

                if self.loop_count > self.max_loop_count:
                    self.loop_count = 0
                    return -1

                time.sleep(0.2 * self.loop_count)
                return self.__get_next_worker_id()
            time.sleep(0.2)
            return self.__get_next_worker_id()
        if is_available(cur):
            self.redis_impl.setex(
                f"IdGen:WorkerId:Value:{cur}",
                self.worker_id_expire_time,
                "Y"
            )
            self.loop_count = 0
            return cur

        return self.__get_next_worker_id()
