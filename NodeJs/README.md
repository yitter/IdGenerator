<!--
 * @description: README 
 * @author: bubao
 * @date: 2021-04-28 09:52:24
 * @last author: bubao
 * @last edit time: 2021-04-28 10:34:31
-->

# README

该代码参考 go 版本，需要 Nodejs 或者浏览器支持 `BigInt`。基本参数与 go 版本一致，只要，但是只实现了雪花飘移算法，没有传统的雪花算法，所以`Method`参数并没有作用。

## 使用

```js
const GenId = require("./index.js")
const genid = new GenId({ WorkerId: 1 });

for (let index = 0; index < 5000; index++) {
    console.log(genid.NextId());
}
```

## 测试

在 Nodejs 环境中，提供了 redis 测试环境，需要在该 README 同级目录下创建一个`env.config.js`文件。

```js
// env.config.js
module.exports = {
    port: 6379, // Redis port
    host: "localhost", // Redis host
    family: 4, // 4 (IPv4) or 6 (IPv6)
    // password: "auth",
    db: 8,
};
```

并安装依赖

```js
npm i
```

测试

```js
npm run test-process
```

看到下面内容则证明测试提供

```sh
50000
parent exit.
```

其中`50000`是生成 50000 条 id。
