#  ❄ idgenerator-JavaScript

## 介绍

项目更多介绍参照：https://github.com/yitter/idgenerator

代码贡献者：bubao 布宝

## 使用

```js
const GenId = require("./index.js")
const genid = new GenId({ WorkerId: 1 });

for (let index = 0; index < 5000; index++) {
    console.log(genid.NextId());
}
```

## function

### function NextId()
根据输出数值判断，小于number最大值时输出number类型，大于时输出bigint

### function NextNumber()
始终输出number类型，超过时throw error

### function NextBigId()
始终输出bigint类型


## 执行测试代码

```bash
node test/test2.js
```

## 其他帮助

在mysql中int类型最大长度是10位数字，由于本算法默认生成的是15位，最短也是11位，所以在mysql中需要使用bigint数据类型
