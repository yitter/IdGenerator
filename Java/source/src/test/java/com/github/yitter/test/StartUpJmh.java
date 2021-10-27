package com.github.yitter.test;

import com.github.yitter.contract.IdGeneratorOptions;
import com.github.yitter.idgen.YitIdHelper;
import java.util.concurrent.TimeUnit;
import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Level;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.annotations.Scope;
import org.openjdk.jmh.annotations.Setup;
import org.openjdk.jmh.annotations.State;
import org.openjdk.jmh.annotations.Threads;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;

/**
 * @author suzhenyu
 * @date 2021/9/27
 */
// 测试方法平均执行时间
@BenchmarkMode({Mode.All})
// 输出结果的时间粒度为微秒
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Thread)
//@Threads(2)
public class StartUpJmh {

  //1-漂移算法，2-传统算法
  final static short method = 1;

  public static void main(String[] args) throws RunnerException {
    Options options = new OptionsBuilder().include(StartUpJmh.class.getSimpleName())
        .warmupIterations(1).measurementIterations(5).forks(1).build();
    new Runner(options).run();
  }

  /**
   * setup初始化容器的时候只执行一次
   */
  @Setup(Level.Trial)
  public void init() {
    IdGeneratorOptions options = new IdGeneratorOptions();
        options.WorkerIdBitLength = 6;
        options.SeqBitLength = 10;
        options.BaseTime = System.currentTimeMillis();
    options.Method = method;
    options.WorkerId = 1;

    // 首先测试一下 IdHelper 方法，获取单个Id
    YitIdHelper.setIdGenerator(options);
    YitIdHelper.nextId();
  }

  @Benchmark
  public void testNextId() {
      YitIdHelper.nextId();
  }
}
