import std.stdio;

import yitter;
import GenTest;

import core.thread;

import std.conv;
import std.datetime;
import std.stdio;

/**
 * 测试结果：
 * (1)：1W并发，方法 1只要 1ms.而方法 2 要 180ms。
 * (2)：5W并发，方法 1只要 3ms.而方法 2 要 900ms。
 * [不同CPU可能结果有差异，但相对大小不变]
 * 默认配置下，最佳性能是5W/s-8W/s
 */
enum int genIdCount = 50000;

//1-漂移算法，2-传统算法
enum short method = 1;

void main()
{

	IdGeneratorOptions options = new IdGeneratorOptions();

	options.Method = method;
	options.BaseTime = SysTime(DateTime(2020, 2, 20, 21, 51, 33));
	options.WorkerId = 1;

	IIdGenerator idGen = new DefaultIdGenerator(options);
	GenTest.GenTest genTest = new GenTest.GenTest(idGen, genIdCount, options.WorkerId);

	// 首先测试一下 IdHelper 方法，获取单个Id
	YitIdHelper.setIdGenerator(options);
	long newId = YitIdHelper.nextId();
	writeln("=====================================");
	writeln("这是用方法 " ~ method.to!string() ~ " 生成的 Id：" ~ newId.to!string());

	// 然后循环测试一下，看看并发请求时的耗时情况
	try {
		while (true) {
			genTest.GenStart();
			// Thread.sleep(200.msecs); // 每隔1秒执行一次GenStart
			// writeln("Hello World! D");
		}
	} catch (Exception e) {
		writeln(e.toString());
	}	
}
