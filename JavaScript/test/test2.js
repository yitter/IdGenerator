const GenId = require('..')


function test1() {
    const genid = new GenId({ WorkerId: 1 })
    for (let index = 0; index < 5000; index++) {
        console.log(genid.NextId());
    }
}
function test2() {
    // const genid = new GenId({ WorkerId: 1, SeqBitLength: 12 })
    const genid = new GenId({ WorkerId: 1 })
    for (let i = 0; i < 10; i++) {
        // let id1 = genid.NextId()
        let id1 = genid.NextBigId()
        // let id1 = genid.NextNumber()
        console.log(`${i} ID:${id1} ${typeof id1} 长度：${id1.toString().length}`)
    }
}

function main() {
    test2()
}
main()