#  ❄ idgenerator-CPP

## 编译说明

跨平台，只需要该系统有C++11或以上编译器就可以编译使用。

## 使用方式
只需要包含 IdGenerator.h 头文件即可
#include "idgen/IdGenerator.h"

## 调用示例（CPP）

第1步，**全局** 初始化（应用程序启动时执行一次，实际上代码保证了初始化只能运行一次）：
```c
// 初始化方式1：
idgen::IdGeneratorOptions options;
options.Method = method;
options.WorkerId = 1; // Your_Unique_Worker_Id
options.SeqBitLength = 10; // 默认值6，限制每毫秒生成的ID个数。若生成速度超过5万个/秒，建议加大 SeqBitLength 到 10。
// options.WorkerIdBitLength = 10; // 默认值6，限定 WorkerId 最大值为2^6-1，即默认最多支持64个节点。
// options.BaseTime = Your_Base_Time; // 如果要兼容老系统的雪花算法，此处应设置为老系统的BaseTime。
idgen::IdGenerator::CreateInstance(options);

// 初始化方式2:
// 或者如果使用默认的options参数, 只需要
//idgen::IdGenerator::CreateInstance(Your_Unique_Worker_Id);

// 以上过程只需全局一次，且应在生成ID之前完成。
```

第2步，生成ID：
```c
// 初始化后，在任何需要生成ID的地方，调用以下方法：
long newId = idgen::IdGenerator::NextId();
```

## 编译示例（CPP）
```shell
如果c++编译器默认是c++11以前的，但支持c++11, 编译的时候需要加上 -std=c++11， 类似：
g++ -std=c++11  -pthread -g -W -O2 main.cpp  -o testcpp

如果c++编译器默认就是c++11或以上的，只需类似：
g++ -pthread -g -W -O2 main.cpp  -o testcpp

编译完毕后，可以运行，例如：
./testcpp > testcpp.log


```
