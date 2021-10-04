#  ❄ idgenerator-TypeScript

## 介绍

项目更多介绍参照：https://github.com/yitter/idgenerator

代码贡献者：zhupengfei（在 bubao 布宝 的JS基础上改版，感谢bubao 布宝）


执行测试代码

```bash
ts-node test/test1.ts
```



## 使用

```js
import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

const Method = process.env.Method == undefined ? 1 : process.env.Method

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId, Method: Method })
let id1 = gen1.NextId()
console.log(id1, id1.toString().length)

```


