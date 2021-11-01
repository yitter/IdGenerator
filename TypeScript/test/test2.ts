import { snowflakeIdv1 } from '../snowflakeIdv1'

const WorkerId = process.env.WorkerId == undefined ? 1 : process.env.WorkerId

const Method = process.env.Method == undefined ? 1 : process.env.Method


console.log("WorkerId:" + WorkerId)
console.log("Method:" + Method)
console.log("--------------------")
let gen1 = new snowflakeIdv1({ workerId: 1 })



function test1() {
    let id1 = gen1.NextId()
    console.log(id1, id1.toString().length)
}

function test2() {
    let id1 = gen1.NextId()
    console.log(typeof (id1))
    console.log(id1, id1.toString().length)
}

function main() {
    test2()
}
main()




