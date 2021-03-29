/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.github.yitter.contract;

public interface ISnowWorker {
    long nextId() throws IdGeneratorException;
}
