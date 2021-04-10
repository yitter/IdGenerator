# 雪花算法里最好用的主键ID生成器


## 技术支持

QQ群：646049993


## 为什么用雪花ID？

❄ 因为大厂也在用，推特、百度、美团、滴滴等等。

❄ 雪花ID是走向分布式架构的垫脚石，如果只会Guid和数据库自增，怎敢说会分布式系统架构。

❄ 雪花ID适合小项目、大项目、超级大项目。


## 💎 本算法介绍

❄ 这是一个优化的雪花算法（雪花漂移），生成的ID更短、速度更快。

❄ 原生支持 C#/Java/Go/Rust/C 等语言，并提供 PHP、Python、Node.js、Ruby 等语言多线程安全调用库(FFI)。

❄ 支持 k8s 等容器环境自动扩容（自动注册 WorkerId）。

❄ 可在单机或分布式环境中生成唯一ID。

❄ 这是计算机历史上最全面的雪花ID生成器，未来会超越自己。（目前还未发现更好的，或许你可以😀）


## 需求来源

💧  作为架构设计的你，想要解决数据库主键唯一的问题，特别是在分布式系统多数据库的时候。

💧  你希望这个主键是用最少的存储空间，索引速度更快，Select、Insert 和 Update 更迅速。

💧  你要考虑在分库分表（合库合表）时，主键值可直接使用，并能反映业务时序。

💧  如果这样的主键值太长，超过前端 js Number 类型最大值，须把 Long 型转换为 String 型，你会觉得有点沮丧。

💧  尽管 Guid 能自增，但占用空间大，索引速度慢，你也不想用它。

💧  应用实例可能超过50个，每个并发请求可达10W/s。

💧  在容器环境部署应用（水平扩展、自动扩容）。

💧  不想依赖 redis 的自增操作。

💧  你希望系统运行 100 年以上。


## 传统算法问题

❌ 生成的ID太长。

❌ 瞬时并发量不够。

❌ 不能解决时间回拨问题。

❌ 不支持后补生成前序ID。

❌ 可能依赖外部存储系统。


## 新算法特点

✔ 整形数字，随时间单调递增（不一定连续），长度更短，用50年都不会超过 js Number类型最大值。（默认配置）

✔ 速度更快，是传统雪花算法的2-5倍，0.1秒可生成50万个（基于8代低压i7）。

✔ 支持时间回拨处理。比如服务器时间回拨1秒，本算法能自动适应生成临界时间的唯一ID。

✔ 支持手工插入新ID。当业务需要在历史时间生成新ID时，用本算法的预留位能生成5000个每秒。

✔ 不依赖任何外部缓存和数据库。（k8s环境下自动注册 WorkerId 的动态库依赖 redis）

✔ 基础功能，开箱即用，无需配置文件、数据库连接等。


## 性能数据

(参数：10位自增序列，1000次漂移最大值)
| 连续请求量 |  5K  |  5W  |  50W  |
|  ----  | ----  |  ----  | ----  |
| 传统雪花算法 | 0.0045s | 0.053s  |  0.556s |
| 雪花漂移算法  | 0.0015s | 0.012s |  0.113s |

💍 极致性能：500W/s~3000W/s。（所有测试数据均基于8代低压i7计算）


## 如何处理时间回拨

🔶 当发生系统时间回拨时，算法采用过去时序的预留序数生成新的ID。

🔶 回拨生成的ID序号，默认靠前，也可以调整为靠后。

🔶 允许时间回拨至本算法预设基数（参数可调）。


## 💎 ID组成

 * 本算法生成的ID由3部分组成（沿用雪花算法定义）：
 * +-------------------------+--------------+----------+
 * | 1.相对基础时间的时间差 | 2.WorkerId | 3.序列数 |
 * +-------------------------+--------------+----------+
 * 
 * 第1部分，时间差，是生成ID时的系统时间减去 BaseTime 的总时间差（毫秒单位）。
 * 第2部分，WorkerId，是区分不同机器或不同应用的唯一ID，最大值由 WorkerIdBitLength（默认6）限定。
 * 第3部分，序列数，是每毫秒下的序列数，由参数中的 SeqBitLength（默认6）限定。

## 💎 ID示例

🟣 本算法生成的 ID ，是一串整数，最多8字节。以下是基于默认配置生成的ID：
```
129053495681099        (本算法运行1年)
387750301904971        (运行3年)
646093214093387        (运行5年)
1292658282840139       (运行10年)
9007199254740992       (js Number 最大值)
165399880288699493     (普通雪花算法生成的ID)
```

🟣 本算法生成的 ID 值，是 js Number 最大值的 1%-10%，是普通雪花算法值的千分之一，而计算能力却超过普通雪花算法。

🟣 js Number 类型最大数值：9007199254740992，本算法在保持并发性能（5W+/0.01s）和最大64个 WorkerId（6bit）的同时，能用70年才到 js Number Max 值。


### 长度估算

```
💍 每增加 1位 WorkerIdBitLength 或 SeqBitLength，生成的ID数字值将会乘以2（基础长度可参考前“ID示例”），反之则除以2。
```

### 能用多久

🔵 在默认配置下，ID可用 71000 年不重复。

🔵 在支持 1024 个工作节点时，ID可用 4480 年不重复。

🔵 在支持 4096 个工作节点时，ID可用 1120 年不重复。


## 💎 参数设置

❄ ***WorkerIdBitLength***，机器码位长，决定 WorkerId 的最大值，默认值6，取值范围 [1, 19]，实际上有些语言采用 无符号ushort(uint16) 类型接收该参数，所以最大值是16，如果是采用有符号short(int16)，则最大值为15。

❄ ***WorkerId***，机器码，**最重要参数**，无默认值，必须由外部设定，默认条件下最大值63，理论最大值 2^WorkerIdBitLength-1（实际上不同实现语言可能会限定在 65535 或 32767，原理同 WorkerIdBitLength 的规则）。不同机器或不同应用实例不可相同，你可通过应用程序配置该值，也可通过调用外部服务获取值。针对自动注册WorkerId需求，本算法提供默认实现：通过 redis 自动注册 WorkerId 的动态库，详见“Tools\AutoRegisterWorkerId”。

❄ ***SeqBitLength***，序列数位长，默认值6，取值范围 [3, 21]（建议不小于4），决定每毫秒生成的 ID 个数。规则要求：WorkerIdBitLength + SeqBitLength 不超过 22。

❄ ***MinSeqNumber***，最小序列数，默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位。

❄ ***MaxSeqNumber***，最大序列数，设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1），不为0时，用该设置值为最大序列数，一般无需设置最大序列数，除非多机共享WorkerId分段生成ID（此时还要正确设置最小序列数）。


## 💎 常规集成

1️⃣ 用单例模式调用。外部集成方使用更多的实例并行调用本算法，不会增加ID产出效能，因为本算法采用单线程模式生成ID。

2️⃣ 指定唯一的 WorkerId。必须由外部系统确保 WorkerId 的全局唯一性，并赋值给本算法入口方法。

3️⃣ 单机多实例部署时使用不同 WorkerId。并非所有实现都支持跨进程的并发唯一，保险起见，在同一主机上部署多应用实例时，请确保各 WorkerId 唯一。

4️⃣ 异常处理。算法会抛出所有 Exception，外部系统应 catch 异常并做好应对处理，以免引发更大的系统崩溃。

5️⃣ 认真理解 IdGeneratorOptions 的定义，这对集成和使用本算法有帮助。

6️⃣ 使用雪花漂移算法。虽然代码里包含了传统雪花算法的定义，并且你可以在入口处指定（Method=2）来启用传统算法，但仍建议你使用雪花漂移算法（Method=1，默认的），毕竟它具有更好的伸缩力和更高的性能。

7️⃣ 不要修改核心算法。本算法内部参数较多，逻辑较为复杂，在你尚未掌握核心逻辑时，请勿尝试修改核心代码且用于生产环境，除非通过大量细致、科学的测试验证。


## 💎 配置变更

配置变更指是系统运行一段时间后，再变更运行参数（IdGeneratorOptions选项值），请注意：

🔴 1.最重要的一条原则是：BaseTime **只能往前**（比老值更小、距离现在更远）赋值，原因是往后赋值极大可能产生相同的时间戳。[**不推荐**在系统运行之后调整 BaseTime]

🔴 2.任何时候增加 WorkerIdBitLength 或 SeqBitLength，都是可以的，但是慎用 “减小”的操作，因为这可能导致在未来某天生成的 ID 与过去老配置时相同。[允许在系统运行之后**增加**任何一个 BitLength 值]

🔴 3.如果必须减小 WorkerIdBitLength 或 SeqBitLength 其中的一项，一定要满足一个条件：新的两个 BitLength 之和要大于 老的值之和。[**不推荐**在运行之后缩小任何一个 BitLength 值]

🔴 4.上述3条规则，并未在本算法内做逻辑控制，集成方应根据上述规则做好影响评估，确认无误后，再实施配置变更。


## 自动注册WorkerId

🔍 唯一ID生成器，依赖WorkerId，当业务服务需要水平无差别复制时，就要求它能自动注册全局唯一WorkerId，然后才能根据它生产唯一ID。

🔍 本算法提供一个开源动态库（go语言实现），能在容器 k8s（或其它容器化集群） 环境下，通过 redis 自动注册 WorkerId。

🔍 通过redis注册WorkerId，并不是唯一的方法。你也可以自己开发一个配置中心服务，各个应用服务启动时，通过配置中心获取唯一 WorkerId。

🔍 当然，如果你的服务不需要自动扩展，你就不必自动注册WorkerId，而是为每个应用手工设定一个唯一值。


#### 自动注册流程图

图片链接：https://gitee.com/yitter/idgenerator/blob/master/Tools/AutoRegisterWorkerId/regprocess.jpg

源码路径：/Go/source/regworkerid/reghelper.go

#### 动态库下载

下载链接：https://gitee.com/yitter/idgenerator/attach_files/662372/download/regworkerid_lib_v1.0.zip

#### 动态库接口定义
```
// 注册一个 WorkerId，会先注销所有本机已注册的记录
// ip: redis 服务器地址
// port: redis 端口
// password: redis 访问密码，可为空字符串“”
// maxWorkerId: 最大 WorkerId
extern GoInt32 RegisterOne(char* ip, GoInt32 port, char* password, GoInt32 maxWorkerId);

// 注销本机已注册的 WorkerId
extern void UnRegister();

// 检查本地WorkerId是否有效（0-有效，其它-无效）
extern GoInt32 Validate(GoInt32 workerId);
```

## 已实现的语言

| 语言 |  github  |  gitee  | 
|  ----  | ----  |  ----  |
| 🌲 C# | [查看示例][1]  | [查看示例][11]  | 
| 🌲 Java | [查看示例][2]  | [查看示例][21]  |  
| 🌲 Go| [查看示例][3]  | [查看示例][31]  |  
| 🌲 Rust | [查看示例][4]  | [查看示例][41]  |  
| 🌲 C | [查看示例][5]  | [查看示例][51]  |  
| 🌲 C (PHP扩展) | [查看示例][7]  | [查看示例][71]  |  
| 🌲 V | [查看示例][6]  | [查看示例][61]  |  
| 🌲 D | [查看示例][72]  | [查看示例][73]  |  


## 为什么不用大厂的？

❄ 首先，大厂们不但自己用雪花ID，而且还开源：[百度][81] | [美团][82] | [滴滴][83] | [雪花ID鼻祖-推特][80]。

❄ 然而，大厂的雪花算法分为“经典算法”和“号段算法”两种，其中“号段算法”依赖网络或外部存储系统，不适合“非大厂”。

❄ 至于其“经典算法”，在“ID长度和生成性能”方面，未做过优化，而这正式本算法——雪花漂移算法的核心所在。


[1]: https://github.com/yitter/idgenerator/tree/master/C%23.NET
[2]: https://github.com/yitter/idgenerator/tree/master/Java
[3]: https://github.com/yitter/idgenerator/tree/master/Go
[4]: https://github.com/yitter/idgenerator/tree/master/Rust
[5]: https://github.com/yitter/idgenerator/tree/master/C
[6]: https://github.com/yitter/idgenerator/tree/master/zzz-OtherLanguages/V
[7]: https://github.com/yitter/idgenerator/tree/master/PHP

[11]: https://gitee.com/yitter/idgenerator/tree/master/C%23.NET
[21]: https://gitee.com/yitter/idgenerator/tree/master/Java
[31]: https://gitee.com/yitter/idgenerator/tree/master/Go
[41]: https://gite.com/yitter/idgenerator/tree/master/Rust
[51]: https://gitee.com/yitter/idgenerator/tree/master/C
[61]: https://gitee.com/yitter/idgenerator/tree/master/zzz-OtherLanguages/V
[71]: https://gitee.com/yitter/idgenerator/tree/master/PHP

[72]: https://github.com/yitter/idgenerator/tree/master/zzz-OtherLanguages/D
[73]: https://gitee.com/yitter/idgenerator/tree/master/zzz-OtherLanguages/D

[80]: https://github.com/twitter-archive/snowflake
[81]: https://github.com/baidu/uid-generator
[82]: https://github.com/Meituan-Dianping/Leaf
[83]: https://github.com/didi/tinyid

