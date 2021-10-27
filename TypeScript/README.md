#  ❄ idgenerator-TypeScript

## 介绍

项目更多介绍参照：https://github.com/yitter/idgenerator

代码贡献者：zhupengfei（在 bubao 布宝 的JS基础上改版，感谢bubao 布宝）

js Number 类型最大数值：9007199254740992(16位)，

在JS中没有bigint类型，所以建议将ID控制在16位以内，统一使用number类型


执行测试代码

```bash
ts-node test/test1.ts
```



## 使用

```js
import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId})
let id1 = gen1.NextId()
console.log(id1, id1.toString().length)

```


## 示例

```js
import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId, SeqBitLength: 6 })
for (let i = 0; i < 10; i++) {
    let id1 = gen1.NextId()
    console.log(`${i} ID:${id1} 长度：${id1.toString().length}`)
}

$ ts-node test/test4.ts
0 ID:234712552579141 长度：15
1 ID:234712552587333 长度：15
2 ID:234712552587334 长度：15
3 ID:234712552587335 长度：15
4 ID:234712552587336 长度：15
5 ID:234712552591429 长度：15
6 ID:234712552591430 长度：15
7 ID:234712552591431 长度：15
8 ID:234712552591432 长度：15
9 ID:234712552591433 长度：15


import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId, SeqBitLength: 11 })
for (let i = 0; i < 10; i++) {
    let id1 = gen1.NextId()
    console.log(`${i} ID:${id1} ${typeof id1} 长度：${id1.toString().length}`)
}


$ ts-node test/test4.ts
0 ID:7510958933018629 number 长度：16
1 ID:7510958933280773 number 长度：16
2 ID:7510958933280774 number 长度：16
3 ID:7510958933280775 number 长度：16
4 ID:7510958933411845 number 长度：16
5 ID:7510958933411846 number 长度：16
6 ID:7510958933542917 number 长度：16
7 ID:7510958933542918 number 长度：16
8 ID:7510958933542919 number 长度：16
9 ID:7510958933673989 number 长度：16

import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId, SeqBitLength: 12 })
for (let i = 0; i < 10; i++) {
    let id1 = gen1.NextId()
    console.log(`${i} ID:${id1} ${typeof id1} 长度：${id1.toString().length}`)
}

$ ts-node test/test4.ts
0 ID:15021931987734533 bigint 长度：17
1 ID:15021931987996677 bigint 长度：17
2 ID:15021931987996678 bigint 长度：17
3 ID:15021931987996679 bigint 长度：17
4 ID:15021931987996680 bigint 长度：17
5 ID:15021931988258821 bigint 长度：17
6 ID:15021931988258822 bigint 长度：17
7 ID:15021931988258823 bigint 长度：17
8 ID:15021931988258824 bigint 长度：17
9 ID:15021931988520965 bigint 长度：17


import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId, SeqBitLength: 13 })
for (let i = 0; i < 10; i++) {
    let id1 = gen1.NextId()
    console.log(`${i} ID:${id1} ${typeof id1} 长度：${id1.toString().length}`)
}

$ ts-node test/test4.ts
0 ID:30043877337997317 bigint 长度：17
1 ID:30043877338521605 bigint 长度：17
2 ID:30043877338521606 bigint 长度：17
3 ID:30043877339045893 bigint 长度：17
4 ID:30043877339045894 bigint 长度：17
5 ID:30043877339045895 bigint 长度：17
6 ID:30043877339045896 bigint 长度：17
7 ID:30043877339570181 bigint 长度：17
8 ID:30043877339570182 bigint 长度：17
9 ID:30043877339570183 bigint 长度：17
















```

## 同时兼容number和bigint的写法

如果您觉得这个用法更好，可以手动替换对应方法

```js
    /**
     * 生成ID
     * @returns 
     */
    public NextId(): number | bigint {
        if (this._IsOverCost) {
            //
            let id = this.NextOverCostId()
            if (id >= 9007199254740992n)
                return id
            else
                return parseInt(id.toString())
        } else {
            //
            let id = this.NextNormalId()
            if (id >= 9007199254740992n)
                return id
            else
                return parseInt(id.toString())
        }
    }

```

## 其他帮助

在mysql中int类型最大长度是10位数字，由于本算法默认生成的是15位，最短也是11位，所以在mysql中需要使用bigint数据类型


