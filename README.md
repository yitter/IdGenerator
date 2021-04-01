# 唯一ID生成器 IdGenerator  SnowFlake 雪花算法 原生多语言

## 介绍

1.一个全新的雪花漂移算法，生成的ID更短、速度更快。 

2.核心在于缩短ID长度的同时，具有极高瞬时并发处理量（保守值 50W/0.1s）。 

3.原生支持 C#/Java/Go/Rust/C 等语言，并由 Rust 提供 PHP、Python、Node.js、Ruby 等语言多线程安全调用库(FFI)。

4.支持 k8s 等容器化部署，自动注册 WorkerId。


## 技术支持

开源地址1：https://gitee.com/yitter/idgenerator

开源地址2：https://github.com/yitter/idgenerator

QQ群：646049993

## 需求来源

1.作为架构设计的你，想要解决数据库主键唯一的问题，特别是在分布式系统多数据库的时候。

2.你希望这个主键是用最少的存储空间，索引速度更快，Select、Insert 和 Update 更迅速。

3.你要考虑在分库分表（合库合表）时，主键值可直接使用，并能反映业务时序。

4.如果这样的主键值太长，超过前端 JS Number 类型最大值，须把 Long 型转换为 String 型，你会觉得有点沮丧。

5.尽管 Guid 能自增，但占用空间大，索引速度慢，你也不想用它。

6.应用实例可能超过50个，每个并发请求可达10W/s。

7.在容器环境部署应用（水平扩展、自动伸缩）。

8.不想依赖 redis 的自增操作。

9.你希望系统运行 100 年以上。


## 传统算法问题

1.生成的ID太长。

2.瞬时并发量不够。

3.不能解决时间回拨问题。

4.不支持后补生成前序ID。

5.依赖外部存储系统。


## 新算法特点

1.整形数字，随时间单调递增（不一定连续），长度更短，用50年都不会超过 js Number类型最大值。（默认配置 WorkerId 是6bit，自增数是6bit）

2.速度更快，是传统雪花算法的2-5倍，0.1秒可生成50万个。（i7笔记本，默认算法配置6bit+6bit）

3.支持时间回拨处理。比如服务器时间回拨1秒，本算法能自动适应生成临界时间的唯一ID。

4.支持手工插入新ID。当业务需要在历史时间生成新ID时，用本算法的预留位能生成5000个每秒。

5.漂移时能外发通知事件。让调用方确切知道算法漂移记录，Log并发调用量。

6.不依赖任何外部缓存和数据库。（k8s环境下自动注册 WorkerId 的动态库依赖 redis）

7.基础功能，开箱即用，无需配置文件、数据库连接等。


## 性能数据
(参数：10位自增序列，1000次漂移最大值)
| 连续请求量 |  5K  |  5W  |  50W  |
|  ----  | ----  |  ----  | ----  |
| 传统雪花算法 | 0.0045s | 0.053s  |  0.556s |
| 雪花漂移算法  | 0.0015s | 0.012s |  0.113s |

## 效果

1.js Number 类型最大数值：9007199254740992，本算法在保持并发性能（5W+/0.01s）和最大64个 WorkerId（6bit）的同时，能用70年才到 js Number Max 值。

2.增加WorkerId位数到8bit（256节点）时，15年达到 js Number Max 值。

3.极致性能：500W/s~3000W/s。

4.所有测试数据均基于8代低压i7计算。

#### 生成的ID

默认配置：
```
WorkerIdBitLength = 6
SeqBitLength = 6
```

ID示例（基于默认配置）：
```
129053495681099        (本算法运行1年)
387750301904971        (运行3年)
646093214093387        (运行5年)
1292658282840139       (运行10年)
9007199254740992       (js Number 最大值)
165399880288699493     (普通雪花算法生成的ID)
```
本算法生成的 ID 值，是 js Number 最大值的 1%-10%，是普通雪花算法值的千分之一，而计算能力却超过普通雪花算法。


## 适用范围

1.小型、中型、大型需要全局唯一Id（不用Guid）的项目。

2.分布式项目。

3.不想将 Long 型转 String 给前端用的项目。（若前端支持bigint，则可不转类型）


## 如何处理时间回拨
1.当发生系统时间回拨时，算法采用过去时序的预留序数生成新的ID。

2.默认每秒生成100个（速度可调整）。

3.回拨生成的ID序号，默认靠前，也可以调整为靠后。

4.允许时间回拨至本算法预设基数（参数可调）。


## 能用多久

1.在默认配置下，ID可用 71000 年不重复。

2.在支持 1024 个工作节点时，ID可用 4480 年不重复。

3.在支持 4096 个工作节点时，ID可用 1120 年不重复。

4.以上所有工作节点，均拥有 50W/0.1s 瞬时处理速度。

#### 默认配置

1.WorkerIdBitLength=6，能支持64个 WorkerId，编号0~63。

2.可通过减少 WorkerIdBitLength 到1~4（为4时最大支持WorkerId为2^4=16个），以减少Id长度。

3.SeqBitLength=6，能支持每秒并发5W请求时，平均处理速度不超过 0.005 s。（不同语言略有差别，最高性能不超过0.002s，平均不超过0.005s）

4.可通过增加 SeqBitLength，支持更高的每秒并发数。默认配置能很高效地支持每秒 5W 并发请求，若要求更高，可适当增加 SeqBitLength 到 8~16，但这将增加Id长度。


## ★★集成建议★★

#### 常规集成

1.用单例模式调用。外部集成方使用更多的实例并行调用本算法，不会增加ID产出效能，因为本算法采用单线程模式生成ID。

2.指定唯一的 WorkerId。必须由外部系统确保 WorkerId 的全局唯一性，并赋值给本算法入口方法。

3.单机多实例部署时使用不同 WorkerId。并非所有实现都支持跨进程的并发唯一，保险起见，在同一主机上部署多应用实例时，请确保各 WorkerId 唯一。

4.异常处理。算法会抛出所有 Exception，外部系统应 catch 异常并做好应对处理，以免引发更大的系统崩溃。

5.认真理解 IdGeneratorOptions 的定义，这对集成和使用本算法有帮助。

6.使用雪花漂移算法。虽然代码里包含了传统雪花算法的定义，并且你可以在入口处指定（Method=2）来启用传统算法，但仍建议你使用雪花漂移算法（Method=1，默认的），毕竟它具有更好的伸缩力和更高的性能。

7.不要修改核心算法。本算法内部参数较多，逻辑较为复杂，在你尚未掌握核心逻辑时，请勿尝试修改核心代码且用于生产环境，除非通过大量细致、科学的测试验证。


#### 自动注册WorkerId

唯一ID生成器，依赖WorkerId，当业务服务需要水平自动化复制时，就要求它能自动化注册全局唯一WorkerId，然后各个容器化的无差别部署的业务服务，才能根据它生产唯一ID。

本算法提供一个开源的动态库（go语言实现），能在容器 k8s（或其它容器化集群） 环境下，通过 redis 自动注册 WorkerId。动态库提供的C接口方法有：

```

// 注册一个新的WorkerId
extern __declspec(dllexport) GoInt RegisterWorkerId(char* ip, GoInt port, char* password, GoInt maxWorkerId);

// 注销WorkerId
extern __declspec(dllexport) void UnRegisterWorkerId();

// 检查本地WorkerId是否有效
extern __declspec(dllexport) GoUint8 ValidateLocalWorkerId(GoInt workerId);

```

#### redis作用

1.只用于注册 WorkerId ，不用于生产 ID。

2.如果手工指定 WorkerId，即可不依赖 redis。


####  其它分布式集成

1.可增加 WorkerIdBitLength 到最大20，支持 1,048,576 个节点，且不影响上述并发性能。[算法支持]

2.采用中心化 IdGenerator 集群，生成可用 Id 列表，存入 Redis 队列供节点消费。此时64个中心化节点数足够大型互联网项目使用。[需集成方扩展实现]

3.以上2条二选一即可，采用方法2一般是因为不想增加最终 ID 长度，但节点数超过64个。

4.任何加大 WorkerIdBitLength 或 SeqBitLength 的设置，都可能会增加 ID 的长度。

#### 配置变更

配置变更指是系统运行一段时间后，再变更运行参数（IdGeneratorOptions选项值），请注意：

1.最重要的一条原则是：BaseTime **只能往前**（比老值更小、距离现在更远）赋值，原因是往后赋值极大可能产生相同的时间戳。[**不推荐**在系统运行之后调整 BaseTime]

2.任何时候增加 WorkerIdBitLength 或 SeqBitLength，都是可以的，但是慎用 “减小”的操作，因为这可能导致在未来某天生成的 ID 与过去老配置时相同。[允许在系统运行之后**增加**任何一个 BitLength 值]

3.如果必须减小 WorkerIdBitLength 或 SeqBitLength 其中的一项，一定要满足一个条件：新的两个 BitLength 之和要大于 老的值之和。[**不推荐**在运行之后缩小任何一个 BitLength 值]

4.上述3条规则，并未在本算法内做逻辑控制，集成方应根据上述规则做好影响评估，确认无误后，再实施配置变更。


## 代码示例

C#：[查看示例][1]

Java：[查看示例][2]

Go：[查看示例][3]

Rust：[查看示例][4]

C：[查看示例][5]


[1]: https://gitee.com/yitter/idgenerator/tree/master/C%23.NET
[2]: https://gitee.com/yitter/idgenerator/tree/master/Java
[3]: https://gitee.com/yitter/idgenerator/tree/master/Go
[4]: https://gitee.com/yitter/idgenerator/tree/master/Rust
[5]: https://gitee.com/yitter/idgenerator/tree/master/C


