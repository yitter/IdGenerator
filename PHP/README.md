#  ❄ idgenerator-php-extension

## 介绍
项目更多介绍参照：https://github.com/yitter/idgenerator

## PHP环境

* PHP7 or later

## 安装方式

```shell
git clone https://gitee.com/yitter/idgenerator.git
cd idgenerator/PHP
phpize
./configure --with-php-config=/path/php-config
make
make install
```

## 如何使用（PHP）

**配置文件配置参数**：
```shell
//snowdrift.ini
snowdrift.Method=1 // 雪花计算方法,（1-漂移算法|2-传统算法），默认1
snowdrift.BaseTime=1582136402000 //基础时间（ms单位），不能超过当前系统时间
snowdrift.WorkerId=1 //机器码，必须由外部设定，最大值 2^WorkerIdBitLength-1
snowdrift.WorkerIdBitLength=6 //机器码位长，默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
snowdrift.SeqBitLength=6 //序列数位长，默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
snowdrift.MaxSeqNumber=0 //最大序列数（含），设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]
snowdrift.MinSeqNumber=5 //最小序列数（含），默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
snowdrift.TopOverCostCount=2000 //最大漂移次数（含），默认2000，推荐范围 500-20000（与计算能力有关）
```

**函数签名**：
```php
\SnowDrift::NextId(int $wid=snowdrift.WorkerId):?int //获取单个id，$wid可选，默认值=snowdrift.WorkerId
\SnowDrift::NextNumId(int $num, int $wid=snowdrift.WorkerId):?array //获取$num个id，$wid可选，默认值=snowdrift.WorkerId
```

**调用示例**
```php
$id=\SnowDrift::NextId();
$id=\SnowDrift::NextId(3);

$ids=\SnowDrift::NextNumId(10000);
$ids=\SnowDrift::NextNumId(10000,3);
```
