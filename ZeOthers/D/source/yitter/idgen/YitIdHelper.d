/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.idgen.YitIdHelper;

import yitter.idgen.DefaultIdGenerator;

import yitter.contract.IIdGenerator;
import yitter.contract.IdGeneratorException;
import yitter.contract.IdGeneratorOptions;

import std.concurrency : initOnce;

/**
 * 这是一个调用的例子，默认情况下，单机集成者可以直接使用 nextId()。
 */
class YitIdHelper {

    private __gshared IIdGenerator idGenInstance = null;

    static IIdGenerator getIdGenInstance() {
        return initOnce!idGenInstance(new DefaultIdGenerator(new IdGeneratorOptions(1)));

    }

    /**
     * 设置参数，建议程序初始化时执行一次
     */
    static void setIdGenerator(IdGeneratorOptions options) {
        idGenInstance = new DefaultIdGenerator(options);
    }

    /**
     * 生成新的Id
     * 调用本方法前，请确保调用了 SetIdGenerator 方法做初始化。
     *
     * @return
     */
    static long nextId() {
        return getIdGenInstance().newLong();
    }
}
