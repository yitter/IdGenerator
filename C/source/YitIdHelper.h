/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://gitee.com/yitter/idgenerator
 */
#pragma once

#include "idgen/IdGenOptions.h"


__declspec(dllexport) void __stdcall SetIdGenerator(IdGeneratorOptions options);

__declspec(dllexport) void __stdcall SetWorker(uint32_t workerId);

__declspec(dllexport) uint64_t __stdcall NextId();

