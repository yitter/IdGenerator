<!--
 * @description: README 
 * @author: bubao
 * @date: 2021-04-28 09:52:24
 * @last author: bubao
 * @last edit time: 2021-04-28 10:34:31
-->

# cherry-id

[![NPM version](https://img.shields.io/npm/v/cherry-id.svg)](https://www.npmjs.com/package/cherry-id) [![jaywcjlove/sb](https://jaywcjlove.github.io/sb/lang/english.svg)](README.md)

该代码参考 go 版本，需要 Nodejs 或者浏览器支持 `BigInt`。基本参数与 go 版本一致，只要，但是只实现了雪花飘移算法，没有传统的雪花算法，所以`Method`参数并没有作用。

## 使用

```js
const GenId = require("./index.js")
const genid = new GenId({ WorkerId: 1 });

for (let index = 0; index < 5000; index++) {
    console.log(genid.NextId());
}
```


