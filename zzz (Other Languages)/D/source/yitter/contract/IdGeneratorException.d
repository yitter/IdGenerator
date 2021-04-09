/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
module yitter.contract.IdGeneratorException;

class IdGeneratorException : Exception {

    // this() {
    //     super();
    // }

    this(string message) {
        super(message);
    }

    // this(Throwable cause) {
    //     super(cause);
    // }

    this(string message, Throwable cause) {
        super(message, cause);
    }

    // this(string msgFormat, Object... args) {
    //     super(string.format(msgFormat, args));
    // }

}
