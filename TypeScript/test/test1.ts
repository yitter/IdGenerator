import { SnowflakeIdv1 } from '../snowflakeIdv1'

let gen1 = new SnowflakeIdv1({ workerId: 1 })
let id1 = gen1.NextId()
console.log(`ID:${id1} 长度：${id1.toString().length}`)