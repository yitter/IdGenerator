
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

## 调用示例（Java）

第1步，全局初始化（应用程序启动时执行一次）：
```
// 创建 IdGeneratorOptions 对象，构造函数输入 WorkerId：
IdGeneratorOptions options = new IdGeneratorOptions(1);
// options.WorkerIdBitLength = 10; // WorkerIdBitLength 默认值6，支持的 WorkerId 最大值为2^6-1，若 WorkerId 超过64，可设置更大的 WorkerIdBitLength
// ...... 其它参数设置参考 IdGeneratorOptions 定义，一般来说，只要再设置 WorkerIdBitLength （决定 WorkerId 的最大值）。

// 保存参数（必须的操作，否则以上设置都不能生效）：
YitIdHelper.setIdGenerator(options);
// 以上初始化过程只需全局一次，且必须在第2步之前设置。
```

第2步，生成ID：
```
// 初始化以后，即可在任何需要生成ID的地方，调用以下方法：
long newId = YitIdHelper.nextId();
```

如果基于DI框架集成，可以参考 YitIdHelper 去管理 IdGenerator 对象，须使用**单例**模式。


