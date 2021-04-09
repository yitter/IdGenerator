/*
 * 版权属于：yitter(yitter@126.com)
 * 代码翻译：amuluowin
 * 代码修订：yitter
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "idgen/SnowWorkerM1.h"
#include "idgen/IdGenerator.h"
#include "YitIdHelper.h"


const int GenIdCount = 50000;
const bool multiThread = false;
const int threadCount = 50;
const int method = 1;

void RunMultiThread() {
    //int64_t start = GetCurrentMicroTime();
    for (int i = 0; i < GenIdCount / threadCount; i++) {
        int64_t id = NextId();
        printf("ID: %D\n", id);
    }

    int64_t end = GetCurrentMicroTime();
    //printf("%s，total：%d μs\n", method == 1 ? "1" : "2", (end - start));
}

void RunSingle() {
    int64_t start = GetCurrentMicroTime();
    for (int i = 0; i < GenIdCount; i++) {
        int64_t id = NextId();
//        printf("ID: %ld\n", id);
    }

    int64_t end = GetCurrentMicroTime();
    printf("%s, total: %d us\n", method == 1 ? "1" : "2", (end - start));
}

int main() {
    IdGeneratorOptions options = BuildIdGenOptions(1);
    options.Method = method;
    options.WorkerId = 1;
    options.SeqBitLength = 6;
    SetIdGenerator(options);

    pthread_t tid[threadCount];
    while (1) {
        if (multiThread) {
            for (int i = 0; i < threadCount; i++) {
                if (pthread_create(&tid[i], NULL, (void *) RunMultiThread, NULL) != 0) {
                    printf("thread creation failed\n");
                    exit(1);
                }
            }
        } else {
            RunSingle();
        }

        sleep(1);
    }
}
