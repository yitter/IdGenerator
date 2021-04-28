/**
 * @description: 
 * @author: bubao
 * @date: 2021-04-27 17:23:36
 * @last author: bubao
 * @last edit time: 2021-04-28 10:05:08
 */

const GenId = require('..')
const Redis = require("ioredis");
const config = require("../config");
const redis = new Redis(config);

const genid = new GenId({ WorkerId: (process.argv[2] || 1) - 0 });

(async () => {
	for (let index = 0; index < 5000; index++) {
		await redis.sadd("setTest", genid.NextId());
	}
	redis.end();
})();