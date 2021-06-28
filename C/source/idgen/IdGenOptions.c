/*
 * 版权属于：yitter(yitter@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 */
#include "IdGenOptions.h"


extern IdGeneratorOptions BuildIdGenOptions(uint32_t workerId) {
    IdGeneratorOptions options;

    options.Method = 1;
    options.BaseTime = 1582136402000;
    options.WorkerId = workerId;
    options.WorkerIdBitLength = 6;
    options.SeqBitLength = 6;
    options.MaxSeqNumber = 0;
    options.MinSeqNumber = 5;
    options.TopOverCostCount = 2000;

    return options;

//    IdGeneratorOptions *options = (IdGeneratorOptions *) malloc(sizeof(IdGeneratorOptions));
//
//    options->Method = 1;
//    options->BaseTime = 1582136402000;
//    options->WorkerId = workerId;
//    options->WorkerIdBitLength = 6;
//    options->SeqBitLength = 6;
//    options->MaxSeqNumber = 63;
//    options->MinSeqNumber = 5;
//    options->TopOverCostCount = 2000;
//
//    return options;
}
