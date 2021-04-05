# idgenerator

## 编译说明

1.默认是 Linux 环境，用 CMake。

2.如果是 Windows 环境，要用 Cygwin 或 MinGW。


## 调用示例（C）

第1步，**全局** 初始化（应用程序启动时执行一次）：
```
// 创建 IdGeneratorOptions 对象，请在构造函数中输入 WorkerId：
IdGeneratorOptions options = BuildIdGenOptions(1);
// options.WorkerIdBitLength = 10; // WorkerIdBitLength 默认值6，支持的 WorkerId 最大值为2^6-1，若 WorkerId 超过64，可设置更大的 WorkerIdBitLength
// ...... 其它参数设置参考 IdGeneratorOptions 定义，一般来说，只要再设置 WorkerIdBitLength （决定 WorkerId 的最大值）。

// 保存参数（必须的操作，否则以上设置都不能生效）：
SetIdGenerator(options);
// 以上初始化过程只需全局一次，且必须在第2步之前设置。
```

第2步，生成ID：
```
// 初始化以后，即可在任何需要生成ID的地方，调用以下方法：
long newId = NextId();
```
