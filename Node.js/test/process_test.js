/**
 * @description: 
 * @author: bubao
 * @date: 2021-04-27 23:38:30
 * @last author: bubao
 * @last edit time: 2021-04-28 10:35:20
 */
const Redis = require("ioredis");
const { spawn } = require("child_process");
const config = require("../env.config.js");
const redis = new Redis(config);
//保存被子进程实例数组
var workers = {};

//这里的被子进程理论上可以无限多
// var appsPath = [__dirname+'/service/clickService.js',__dirname+'/service/showService.js'];

var createWorker = function (appPath, i) {

	//保存spawn返回的进程实例
	var worker = spawn('node', [appPath, i]);

	//监听子进程exit事件
	worker.on('exit', async function () {
		console.info('worker:' + worker.pid + 'exited');
		delete workers[worker.pid];
		// createWorker(appPath);
		if (Object.keys(workers).length === 0) {
			console.log(await redis.scard('setTest'));
			await redis.del("setTest");
			redis.end();
		}
	});
	workers[worker.pid] = worker;
	console.info('create worker:' + worker.pid);
};
redis.del("setTest").then(() => {
	//启动所有子进程
	for (var i = 10; i > 0; i--) {
		createWorker(__dirname + '/test.js', i);
	}
});
//父进程退出时杀死所有子进程
process.on('exit', async function () {
	console.info('parent exit.');
	for (var pid in workers) {
		workers[pid].kill('SIGHUP');
	}
	if (Object.keys(workers).length===0&&redis.status!=="end") {
		console.log(await redis.scard('setTest'));
		await redis.del("setTest");
		redis.end();
	}
});