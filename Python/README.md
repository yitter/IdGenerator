#  ❄ idgenerator-Python（未完成）


## 运行环境

Python 3.6+

## 引用 包

## 调用示例

```python
# 导入包
from source import options,generator
# 声明id生成器参数，需要自己构建一个workerId
options = options.IdGeneratorOptions(workerId=23)
# 参数中，WorkerIdBitLength 默认值6，支持的 WorkerId 最大值为2^6-1，若 WorkerId 超过64，可设置更大的 WorkerIdBitLength
idgen = generator.DefaultIdGenerator()
# 保存参数 
idgen.SetIdGernerator(options)
# 生成id
uid = idgen.NextId()
# 打印出来查看
print("%d, %x" % (uid,uid))
```

