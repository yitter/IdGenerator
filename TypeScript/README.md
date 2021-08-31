#  ❄ idgenerator-TypeScript

## 介绍

项目更多介绍参照：https://github.com/yitter/idgenerator

代码贡献者：zhupengfei（在 bubao 布宝 的JS基础上改版，感谢bubao 布宝）


执行测试代码

```bash
ts-node test/test.ts

NODE_ENV=development REDIS_HOST=127.0.0.1 
```



## 使用

```js
import { Genid } from '../index'


let gen = new Genid({ WorkerId: 1 })
let id1 = gen.NextId()
console.log(id1, id1.toString().length)

```


