# 雪花算法中非常好用的数字ID生成器

## 💎 最佳实践（置顶）

针对大家在测试过程中，经常出现的性能问题，以下给出3组最佳实践配置：

❄ 如果生成ID速度不超过5W/s，不用修改任何配置参数

❄ 如果生成ID速度超过5W/s，低于50W，推荐修改：SeqBitLength=10

❄ 如果生成ID速度超过50W/s，接近500W，推荐修改：SeqBitLength=12

总之，增加 SeqBitLength 会让性能更高，但生成的 ID 也会更长。


## 💎 算法介绍

❄ 这是优化的雪花算法（雪花漂移），它生成的ID更短、速度更快。

❄ 支持 k8s 等容器环境自动扩容（自动注册 WorkerId），可在单机或分布式环境生成数字型唯一ID。

❄ 原生支持 C#/Java/Go/C/Rust/Python/Node.js/PHP(C扩展)/SQL/ 等语言，并提供多线程安全调用动态库（FFI）。

❄ 兼容所有雪花算法（号段模式或经典模式，大厂或小厂），将来你可做任意的升级切换。

❄ 这是计算机历史上最全面的雪花ID生成工具。【截至2022年8月】


#### 需求来源

💧  作为架构设计的你，想要解决数据库主键唯一的问题，特别是在分布式系统多数据库中。

💧  你希望数据表主键用最少的存储空间，索引速度更快，Select、Insert 和 Update 更迅速。

💧  你要考虑在分库分表（合库合表）时，主键值可直接使用，并能反映业务时序。

💧  如果这样的主键值太长，超过前端 js Number 类型最大值，须把 Long 型转换为 String 型，你会觉得有点沮丧。

💧  尽管 Guid 能自增，但占用空间大，索引速度慢，你不想用它。

💧  应用实例可能超过50个，每个并发请求可达10W/s。

💧  要在容器环境部署应用，支持水平复制、自动扩容。

💧  不想依赖 redis 的自增操作获得连续的主键ID，因为连续的ID存在业务数据安全风险。

💧  你希望系统运行 100 年以上。


#### 传统算法问题

❌ 生成的ID太长。

❌ 瞬时并发量不够。

❌ 不能解决时间回拨问题。

❌ 不支持后补生成前序ID。

❌ 可能依赖外部存储系统。


#### 新算法特点

✔ 整形数字，随时间单调递增（不一定连续），长度更短，用50年都不会超过 js Number类型最大值。（默认配置）

✔ 速度更快，是传统雪花算法的2-5倍，0.1秒可生成50万个（基于8代低压i7）。

✔ 支持时间回拨处理。比如服务器时间回拨1秒，本算法能自动适应生成临界时间的唯一ID。

✔ 支持手工插入新ID。当业务需要在历史时间生成新ID时，用本算法的预留位能生成5000个每秒。

✔ 不依赖任何外部缓存和数据库。（k8s环境下自动注册 WorkerId 的动态库依赖 redis）

✔ 基础功能，开箱即用，无需配置文件、数据库连接等。


#### 性能数据

(参数：10位自增序列，1000次漂移最大值)
| 连续请求量 |  5K  |  5W  |  50W  |
|  ----  | ----  |  ----  | ----  |
| 传统雪花算法 | 0.0045s | 0.053s  |  0.556s |
| 雪花漂移算法  | 0.0015s | 0.012s |  0.113s |

💍 极致性能：500W/s~3000W/s。（所有测试数据均基于8代低压i7计算）


#### 如何处理时间回拨

🔶 当发生系统时间回拨时，算法采用过去时序的预留序数生成新的ID。

🔶 回拨生成的ID序号，默认靠前，也可以调整为靠后。

🔶 允许时间回拨至本算法预设基数（参数可调）。


## 💎 ID组成说明

 * 本算法生成的ID由3部分组成（沿用雪花算法定义）：
 * +-------------------------+--------------+----------+
 * | 1.相对基础时间的时间差 | 2.WorkerId | 3.序列数 |
 * +-------------------------+--------------+----------+
 * 
 * 第1部分，时间差，是生成ID时的系统时间减去 BaseTime 的总时间差（毫秒单位）。
 * 第2部分，WorkerId，是区分不同机器或不同应用的唯一ID，最大值由 WorkerIdBitLength（默认6）限定。
 * 第3部分，序列数，是每毫秒下的序列数，由参数中的 SeqBitLength（默认6）限定。

#### ID示例

🟣 本算法生成的 ID ，是整数（占用空间最多8字节），以下是基于默认配置生成的ID：
```
129053495681099        (运行1年，长度：15)
387750301904971        (运行3年，长度：15)
646093214093387        (运行5年，长度：15)
1292658282840139       (运行10年，长度：16)
9007199254740992       (运行70年，达到 js Number 最大值，长度：16)
165399880288699493     (运行1000年，等同普通雪花算法运行1年，长度：18)
```

🟣 本算法生成的 ID 值，是 js Number 最大值的 1%-10%，是普通雪花算法值的千分之一，而生成速度却超过普通雪花算法。

🟣 js Number 类型最大数值：9007199254740992，本算法在保持并发性能（5W+/0.01s）和最大64个 WorkerId（6bit）的同时，能用70年才到 js Number Max 值。


### 长度估算

```
💍 每增加 1位 WorkerIdBitLength 或 SeqBitLength，生成的ID数字值将会乘以2（基础长度可参考前一节“ID示例”），反之则除以2。
```

### 能用多久

能用多久的解释，是指生成的ID数字，何时能增长到超过 long（有符号64位，8字节）最大值。

🔵 在默认配置下，ID可用 71000 年不重复。

🔵 在支持 1024 个工作节点时，ID可用 4480 年不重复。

🔵 在支持 4096 个工作节点时，ID可用 1120 年不重复。


## 💎 参数设置

❄ ***WorkerIdBitLength***，机器码位长，决定 WorkerId 的最大值，**默认值6**，取值范围 [1, 19]，实际上有些语言采用 无符号 ushort (uint16)  类型接收该参数，所以最大值是16，如果是采用 有符号 short (int16)，则最大值为15。

❄ **WorkerId**，机器码，**最重要参数**，无默认值，必须 **全局唯一**（或相同 DataCenterId 内唯一），必须 **程序设定**，缺省条件（WorkerIdBitLength取默认值）时最大值63，理论最大值 2^WorkerIdBitLength-1（不同实现语言可能会限定在 65535 或 32767，原理同 WorkerIdBitLength 规则）。不同机器或不同应用实例 **不能相同**，你可通过应用程序配置该值，也可通过调用外部服务获取值。针对自动注册WorkerId需求，本算法提供默认实现：通过 redis 自动注册 WorkerId 的动态库，详见“Tools\AutoRegisterWorkerId”。

**特别提示**：如果一台服务器部署多个独立服务，需要为每个服务指定不同的 WorkerId。

❄ ***SeqBitLength***，序列数位长，**默认值6**，取值范围 [3, 21]（建议不小于4），决定每毫秒基础生成的ID个数。如果每秒请求数不超过5W，保持默认值6即可；如果超过5W，不超过50W，建议赋值10或更大，以此类推。规则要求：WorkerIdBitLength + SeqBitLength 不超过 22。

❄ ***MinSeqNumber***，最小序列数，默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位。

❄ ***MaxSeqNumber***，最大序列数，设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，真实最大序列数取最大值（2^SeqBitLength-1），不为0时，取其为真实最大序列数，一般无需设置，除非多机共享WorkerId分段生成ID（此时还要正确设置最小序列数）。

❄ ***BaseTime***，基础时间（也称：基点时间、原点时间、纪元时间），有默认值（2020年），是毫秒时间戳（是整数，.NET是DatetTime类型），作用是：用生成ID时的系统时间与基础时间的差值（毫秒数）作为生成ID的时间戳。基础时间一般无需设置，如果觉得默认值太老，你可以重新设置，不过要注意，这个值以后最好不变。


第二版计划增加参数：

❄ ***DataCenterId***，数据中心ID（机房ID，默认0），请确保全局唯一。

❄ ***DataCenterIdBitLength***，数据中心ID长度（默认0）。

❄ ***TimestampType***，时间戳类型（0-毫秒，1-秒），默认0。


#### 常规集成

1️⃣ 用单例模式调用。本算法采用单线程生成ID，多方调用会被互斥。在同一应用实例内，调用者使用多线程（或并行）方式调用本算法，不会增加ID产出速度。

2️⃣ 指定唯一的 WorkerId。必须由外部系统确保 WorkerId 的全局唯一性，并赋值给本算法入口参数。

3️⃣ 单机多实例部署时使用不同 WorkerId。并非所有实现都支持跨进程的并发唯一，保险起见，在同一主机上部署多应用实例时，请确保各 WorkerId 唯一。

4️⃣ 异常处理。算法会抛出所有 Exception，外部系统应 catch 异常并做好应对处理，以免引发更大的系统崩溃。

5️⃣ 认真理解 IdGeneratorOptions 的定义，这对集成和使用本算法有帮助。

6️⃣ 使用雪花漂移算法。虽然代码里包含了传统雪花算法的定义，并且你可以在入口处指定（Method=2）来启用传统算法，但仍建议你使用雪花漂移算法（Method=1，默认的），毕竟它具有更好的伸缩力和更高的性能。

7️⃣ 不要修改核心算法。本算法内部参数较多，逻辑较为复杂，在你尚未掌握核心逻辑时，请勿修改核心代码且用于生产环境，除非通过大量细致、科学的测试验证。

8️⃣ 应用域内配置策略相同。当系统运行一段时间后，项目需要从程序指定 WorkerId 转到自动注册 WorkerId 时，请确保同一应用域内所有在用实例采用一致的配置策略，这不仅仅针对 WorkerId，也包含其他配置参数。

9️⃣ 管理好服务器时间。雪花算法依赖系统时间，不要手工大幅度回调操作系统时间。如果一定要调整，切记：确保服务再次启动时的系统时间大于最后一次关闭时的时间。（注：世界级或网络级的时间同步或回拨，引起的系统时间小幅度变化，对本算法没影响）


#### 配置变更

配置变更是指系统运行一段时间后，再调整运行参数（IdGeneratorOptions 对象属性），请注意：

🔴 1.首要原则是：BaseTime **只能更旧**（距现在更远），让生成的ID值较历史最大值更大，确保没有时间重叠区，不产生重复ID。[**不推荐** 在系统运行之后调整 BaseTime]

🔴 2.任何时候增加 WorkerIdBitLength 或 SeqBitLength，都是允许的，但应慎用 “减小”操作，因为这可能导致在未来某天生成的 ID 与旧配置时相同。[允许在系统运行之后 **增加** 任何一个 xxxBitLength 值]

🔴 3.如果必须减小 WorkerIdBitLength 或 SeqBitLength 其中的一项，一定要满足条件：新的两个 xxxBitLength 之和要大于旧值之和。[**不推荐** 在运行之后缩小任何一个 BitLength 值]

🔴 4.上述3条规则，并未在本算法内做逻辑控制，使用者应在确认新配置符合要求后，再实施配置变更。


## 自动注册WorkerId

🔍 唯一ID生成器，依赖WorkerId，当业务服务需要水平无差别复制（自动扩容）时，这就要求能自动注册全局唯一WorkerId，然后才能生产唯一ID。

🔍 本算法提供开源动态库（go语言实现），能在容器 k8s 等容器环境下，通过 redis 自动注册 WorkerId。

🔍 通过redis注册WorkerId，并非唯一方法。你还可以开发中心化的配置服务，各端点服务启动时，通过中心服务获取唯一 WorkerId。

🔍 当然，如果你的服务无需自动扩容，那就不必自动注册WorkerId，而是为它们分别设置全局唯一值。

🔍 方法还有很多，例如：开发中心化的ID生成服务，由它为各端点服务（单个或批量）生成可用ID。


#### 自动注册流程图

图片链接：https://github.com/yitter/IdGenerator/blob/master/Tools/AutoRegisterWorkerId/regprocess.jpg

源码路径：/Go/source/regworkerid/reghelper.go

#### 动态库下载

下载链接：https://github.com/yitter/IdGenerator/releases/download/v1.3.3/workeridgo_lib_v1.3.3.zip

#### 动态库接口定义
```
// 注册一个 WorkerId，会先注销所有本机已注册的记录
// address: Redis连接地址，单机模式示例：127.0.0.1:6379，哨兵/集群模式示例：127.0.0.1:26380,127.0.0.1:26381,127.0.0.1:26382
// password: Redis连接密码
// db: Redis指定存储库，示例：1
// sentinelMasterName: Redis 哨兵模式下的服务名称，示例：mymaster，非哨兵模式传入空字符串即可
// minWorkerId: WorkerId 最小值，示例：30
// maxWorkerId: WorkerId 最大值，示例：63
// lifeTimeSeconds: WorkerId缓存时长（秒，3的倍数），推荐值15
extern GoInt32 RegisterOne(char* server, char* password, GoInt32 db, char* sentinelMasterName, GoInt32 minWorkerId, GoInt32 maxWorkerId, GoInt32 lifeTimeSeconds);

// 注销本机已注册的 WorkerId
extern void UnRegister();

```

## 已实现的语言

| 语言 |  github  | 
|  ----  | ----  |  
| 🌲 C# | [查看示例][1]  | 
| 🌲 Java | [查看示例][2]  | 
| 🌲 Go| [查看示例][3]  | 
| 🌲 Rust | [查看示例][4]  | 
| 🌲 Python | [查看示例][10]  | 
| 🌲 C | [查看示例][5]  |
| 🌲 C (PHP扩展) | [查看示例][7]  |
| 🌲 Delphi (Pascal) | [查看示例][6]  |
| 🌲 JavaScript | [查看示例][8]  |
| 🌲 TypeScript | [查看示例][9]  |
| 🌲 V | [查看示例][6]  | 
| 🌲 D | [查看示例][72]  | 


## 技术支持

开源地址：https://github.com/yitter/IdGenerator

QQ群：646049993


[comment]: <> (	## 为什么用雪花ID？)
[comment]: <> (	❄ 因为大厂也在用，推特、百度、美团、滴滴等等。)
[comment]: <> (	❄ 雪花ID是走向分布式架构的垫脚石，如果只会Guid和数据库自增，怎敢说会分布式系统架构。)
[comment]: <> (	❄ 雪花ID适合小项目、大项目、超级大项目。)
[comment]: <> (	## 为什么不用大厂的？)
[comment]: <> (	❄ 首先，大厂们不但自己用雪花ID，而且还开源：[百度][81] | [美团][82] | [滴滴][83] | [Sony][84] | [雪花ID鼻祖-推特][80]。)
[comment]: <> (	❄ 然而，大厂的雪花算法分为“经典算法”和“号段算法”两种，其中“号段算法”依赖网络或外部存储系统，不适合“非大厂”，且存在无法反应业务时序的缺点。)
[comment]: <> (	❄ 至于其“经典算法”，在“ID长度和生成性能”方面，未做过优化，而这正式本算法——雪花漂移算法的核心所在。)


[1]: https://github.com/yitter/idgenerator/tree/master/C%23.NET
[2]: https://github.com/yitter/idgenerator/tree/master/Java
[3]: https://github.com/yitter/idgenerator/tree/master/Go
[4]: https://github.com/yitter/idgenerator/tree/master/Rust
[5]: https://github.com/yitter/idgenerator/tree/master/C
[6]: https://github.com/yitter/idgenerator/tree/master/Delphi
[7]: https://github.com/yitter/idgenerator/tree/master/PHP
[8]: https://github.com/yitter/IdGenerator/tree/master/JavaScript
[9]: https://github.com/yitter/IdGenerator/tree/master/TypeScript
[10]: https://github.com/yitter/IdGenerator/tree/master/Python

[11]: https://github.com/yitter/idgenerator/tree/master/C%23.NET
[21]: https://github.com/yitter/idgenerator/tree/master/Java
[31]: https://github.com/yitter/idgenerator/tree/master/Go
[41]: https://github.com/yitter/idgenerator/tree/master/Rust
[51]: https://github.com/yitter/idgenerator/tree/master/C
[61]: https://github.com/yitter/idgenerator/tree/master/Delphi
[71]: https://github.com/yitter/idgenerator/tree/master/PHP

[72]: https://github.com/yitter/idgenerator/tree/master/zzz-OtherLanguages/D
[73]: https://github.com/yitter/idgenerator/tree/master/zzz-OtherLanguages/D

[80]: https://github.com/twitter-archive/snowflake
[81]: https://github.com/baidu/uid-generator
[82]: https://github.com/Meituan-Dianping/Leaf
[83]: https://github.com/didi/tinyid
[84]: https://github.com/sony/sonyflake

