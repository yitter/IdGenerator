/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.yitter.contract;

public interface IIdGenerator {
    long newLong() throws IdGeneratorException;
}
