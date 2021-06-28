#  ❄ idgenerator-Node.js

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


