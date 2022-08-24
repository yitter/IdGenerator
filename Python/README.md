#  ❄ idgenerator-Python


## 运行环境

Python 3.6+

## 引用 包

## 调用示例

​	调用方法如下，其中worker_id为一个全局唯一的数字。

```python
# 导入包
from source import options, generator
# 声明id生成器参数，需要自己构建一个worker_id
options = options.IdGeneratorOptions(worker_id=23)
# 参数中，worker_id_bit_length 默认值6，支持的 worker_id 最大值为2^6-1，若 worker_id 超过64，可设置更大的 worker_id_bit_length
idgen = generator.DefaultIdGenerator()
# 保存参数 
idgen.set_id_generator(options)
# 生成id
uid = idgen.next_id()
# 打印出来查看
print("%d, %x" % (uid,uid))
```

​	包里面也提供了一个基于redis的worker id注册器，使用方法如下：

```python
from source import idregister
# 声明注册器，提供redis地址
register = idregister.Register(host="127.0.0.1", port=6379, max_worker_id=100)
# 获取worker id
worker_id = register.get_worker_id()
# 打印出来查看
print(worker_id)
# 程序退出的时候调用一次stop
register.stop()
```

​	需要注意，注册器会启动一个线程，每隔一定时间向redis续期worker id，可以在最后退出程序的时候调用一次stop函数，使该线程退出，不过这需要等待几秒钟。
