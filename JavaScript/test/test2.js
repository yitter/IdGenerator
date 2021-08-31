const GenId = require('..')


function test1() {
    const genid = new GenId({ WorkerId: 1 })
    for (let index = 0; index < 5000; index++) {
        console.log(genid.NextId());
    }
}
function test2() {
    const genid = new GenId({ WorkerId: 1 })
    const id = genid.NextId()
    console.log(typeof (id))
    console.log(id, id.toString().length)
}

function main() {
    test2()
}
main()