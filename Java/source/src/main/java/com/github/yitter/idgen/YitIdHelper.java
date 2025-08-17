/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 */
package com.github.yitter.idgen;

import java.util.HashMap;
import java.util.Map;

import com.github.yitter.contract.IIdGenerator;
import com.github.yitter.contract.IdGeneratorException;
import com.github.yitter.contract.IdGeneratorOptions;

/**
 * 这是一个调用的例子，默认情况下，单机集成者可以直接使用 nextId()。
 */
public class YitIdHelper {

    private static final String DEFAULT_INSTANCE_NAME = "default";

    private static final Map<String, IIdGenerator> idGenInstanceMap = new HashMap<>();

    public static IIdGenerator getIdGenInstance() {
        return idGenInstanceMap.get(DEFAULT_INSTANCE_NAME);
    }

    public static IIdGenerator getIdGenInstance(String instanceName) {
        return idGenInstanceMap.get(instanceName);
    }

    /**
     * 设置参数，建议程序初始化时执行一次
     */
    public static void setIdGenerator(IdGeneratorOptions options) throws IdGeneratorException {
        idGenInstanceMap.put(DEFAULT_INSTANCE_NAME, new DefaultIdGenerator(options));
    }

    /**
     * 设置参数，建议程序初始化时执行一次
     */
    public static void setIdGenerator(String instanceName, IdGeneratorOptions options) throws IdGeneratorException {
        if (instanceName == null || instanceName.trim().isEmpty()) {
            throw new IdGeneratorException("instanceName cannot be null or empty");
        }

        idGenInstanceMap.put(instanceName, new DefaultIdGenerator(options));
    }

    /**
     * 生成新的Id
     * 调用本方法前，请确保调用了 SetIdGenerator 方法做初始化。
     *
     * @return
     */
    public static long nextId() throws IdGeneratorException {
        // if (idGenInstance == null) {
        // idGenInstance = new DefaultIdGenerator(new IdGeneratorOptions((short) 1));
        // }

        var generator = idGenInstanceMap.get(DEFAULT_INSTANCE_NAME);
        if (generator == null)
            throw new IdGeneratorException("Please initialize Yitter.IdGeneratorOptions first.");

        return generator.newLong();
    }

    /**
     * 生成新的Id
     * 调用本方法前，请确保调用了 SetIdGenerator 方法做初始化。
     *
     * @return
     */
    public static long nextId(String instanceName) throws IdGeneratorException {
        var generator = idGenInstanceMap.get(instanceName);
        if (generator == null)
            throw new IdGeneratorException("Please initialize instance: " + instanceName + " with YitIdHelper.setIdGenerator(instanceName, options) first.");

        return generator.newLong();
    }
}
