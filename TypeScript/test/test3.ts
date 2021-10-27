import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

const Method = process.env.Method == undefined ? 1 : process.env.Method

let gen1 = new snowflakeIdv1({ WorkerId: WorkerId, Method: Method })
let id1 = gen1.NextId()
console.log(id1, id1.toString().length)