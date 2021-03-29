/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#pragma once

#include "idgen/IdGenOptions.h"
#include "idgen/common.h"


TAP_DLLEXPORT
extern void TAP_STDCALL SetIdGenerator(IdGeneratorOptions options);

TAP_DLLEXPORT
extern void TAP_STDCALL SetWorkerId(uint32_t workerId);

TAP_DLLEXPORT
extern uint64_t TAP_STDCALL NextId();

TAP_DLLEXPORT
extern uint64_t TAP_STDCALL TestId();
