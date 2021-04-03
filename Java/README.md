
## 运行环境

JDK 1.8+

## 引用 maven 包
```
<dependency>
	<groupId>com.github.yitter</groupId>
	<artifactId>yitter-idgenerator</artifactId>
	<version>1.0.5</version>
</dependency>
```

## 调用示例

```
// 全局初始化设置WorkerId，默认最大2^16-1，可通过调整 WorkerIdBitLength 增加最大值
IdGeneratorOptions options = new IdGeneratorOptions();
options.WorkerId = 1;
YitIdHelper.setIdGenerator(options);
// 以上初始化过程全局只需一次，且必须最先设置

// 初始化以后，即可在需要生成ID的地方，调用以下方法。
long newId = YitIdHelper.nextId();

```
如果基于DI框架集成，可以参考 YitIdHelper 去管理 IdGenerator 对象，须使用**单例**模式。


## options 默认值及说明

参考源码：/contract/IdGeneratorOptions.java

