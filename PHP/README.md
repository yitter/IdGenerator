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
snowdrift.Method=1 //1 漂移算法 2 传统算法
snowdrift.BaseTime=1582136402000
snowdrift.WorkerId=1 //默认workerid
snowdrift.WorkerIdNum=1 //支持的WorkerId数量，默认1，不超过(-1L << snowdrift.WorkerIdBitLength) ^ -1L
snowdrift.WorkerIdBitLength=6
snowdrift.SeqBitLength=6 //自增序号位数
snowdrift.MaxSeqNumber=0
snowdrift.MinSeqNumber=0
snowdrift.TopOverCostCount=2000 //最大漂移次数
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
