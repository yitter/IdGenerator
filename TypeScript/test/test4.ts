import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ workerId: 1, seqBitLength: 10 })
for (let i = 0; i < 10; i++) {
    let id1 = gen1.NextId()
    console.log(`${i} ID:${id1} ${typeof id1} 长度：${id1.toString().length}`)
}
