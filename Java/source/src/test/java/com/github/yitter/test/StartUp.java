package com.github.yitter.test;

import com.github.yitter.contract.IdGeneratorOptions;
import com.github.yitter.idgen.YitIdHelper;

public class StartUp {

    /**
     * 测试结果：
     * (1)：1W并发，方法 1只要 1ms.而方法 2 要 180ms。
     * (2)：5W并发，方法 1只要 3ms.而方法 2 要 900ms。
     * [不同CPU可能结果有差异，但相对大小不变]
     * 默认配置下，最佳性能是5W/s-8W/s
     */
    final static int genIdCount = 500000;

    //1-漂移算法，2-传统算法
    final static short method = 1;


    public static void main(String[] args) {
        IdGeneratorOptions options = new IdGeneratorOptions();
//        options.WorkerIdBitLength = 6;  // 默认6
       options.SeqBitLength = 10; // 默认6
//        options.BaseTime = 1582206693000L;
        options.Method = method;
        options.WorkerId = 1;
        // options.TopOverCostCount=2000;

        // 首先测试一下 IdHelper 方法，获取单个Id
        YitIdHelper.setIdGenerator(options);
        long newId = YitIdHelper.nextId();
        System.out.println("=====================================");
        System.out.println("这是用方法 " + method + " 生成的 Id：" + newId);

        // 然后循环测试一下，看看并发请求时的耗时情况
        try {
            GenTest genTest = new GenTest(genIdCount, options.WorkerId);
            while (true) {
                genTest.GenStart();
                Thread.sleep(1000); // 每隔1秒执行一次GenStart
                // System.out.println("Hello World! Java");
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

// mvn clean deploy -P release