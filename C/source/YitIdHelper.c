/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#include <stdlib.h>
#include <stdint.h>
#include "YitIdHelper.h"
#include "idgen/IdGenerator.h"

extern void SetIdGenerator(IdGeneratorOptions options) {
    SetOptions(options);
}

extern void SetWorker(uint32_t workerId) {
    IdGeneratorOptions options = BuildIdGenOptions(workerId);
    SetIdGenerator(options);
}

extern uint64_t NextId() {
    return GetIdGenInstance()->NextId();
}

