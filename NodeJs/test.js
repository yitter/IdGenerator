/**
 * @description: 
 * @author: bubao
 * @date: 2021-04-28 00:49:07
 * @last author: bubao
 * @last edit time: 2021-04-28 00:50:50
 */
const GenId = require('./genid')

const genid = new GenId({ WorkerId: 1 });

for (let index = 0; index < 5000; index++) {
	console.log(genid.NextId());
}