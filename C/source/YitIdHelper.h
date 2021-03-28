/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#pragma once

#include "idgen/IdGenOptions.h"
#include "idgen/common.h"


TAP_DLLEXPORT
extern void SetIdGenerator(IdGeneratorOptions options);

TAP_DLLEXPORT
extern void SetWorkerId(uint32_t workerId);

TAP_DLLEXPORT
extern uint64_t NextId();

TAP_DLLEXPORT
extern uint64_t TestId() ;
