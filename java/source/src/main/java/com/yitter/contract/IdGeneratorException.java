/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
package com.yitter.contract;

public class IdGeneratorException extends RuntimeException {

    public IdGeneratorException() {
        super();
    }

    public IdGeneratorException(String message) {
        super(message);
    }

    public IdGeneratorException(Throwable cause) {
        super(cause);
    }
    public IdGeneratorException(String message, Throwable cause) {
        super(message, cause);
    }

    public IdGeneratorException(String msgFormat, Object... args) {
        super(String.format(msgFormat, args));
    }

}
