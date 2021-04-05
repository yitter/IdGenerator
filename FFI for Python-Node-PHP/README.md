# idgenerator

## FFI 调用

Pyton、node.js、PHP 等可以通过 FFI 方式调用动态库生成 ID。

不同操作系统请引用不同的动态库。

## 接口方法说明

第1步，**全局** 初始化（应用程序启动时执行一次）：
```
// 设置参数
// workerId
// workerIdBitLength，影响 workerId最大值，一般设置6。支持的 WorkerId 最大值为2^workerIdBitLength-1
// seqBitLength，一般只要设置6.
extern "C" void SetOptions(int workerId, int workerIdBitLength, int seqBitLength) 
```

第2步，生成ID：
```
// 生成ID
extern "C" long NextId() 
```

