import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId })
let id1 = gen1.NextId()
console.log(`ID:${id1} 长度：${id1.toString().length}`)