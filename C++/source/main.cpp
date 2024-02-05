#include <inttypes.h>
#include <vector>
#include "idgen/IdGenerator.h"

const int GenIdCount = 500000;
const bool multiThread = true;
const int threadCount = 50;
const int method = 1;

void RunMultiThread()
{
    int64_t start = idgen::GetCurrentMicroTime();
    for (int i = 0; i < GenIdCount / threadCount; i++)
    {
        int64_t id = idgen::IdGenerator::NextId();
        printf("ID: %" PRId64 "\n", id);
    }

    int64_t end = idgen::GetCurrentMicroTime();
    printf("%s，total：%" PRId64 " μs\n", method == 1 ? "1" : "2", (end - start));
}

void RunSingle()
{
    int64_t start = idgen::GetCurrentMicroTime();
    for (int i = 0; i < GenIdCount; i++)
    {
        int64_t id = idgen::IdGenerator::NextId();
        printf("ID: %" PRId64 "\n", id);
    }

    int64_t end = idgen::GetCurrentMicroTime();

    printf("%s, total: %" PRId64 " us\n", method == 1 ? "1" : "2", (end - start));
}

void create_instance_fun()
{
    idgen::IdGeneratorOptions options;
    options.Method = method;
    options.WorkerId = 1;
    options.SeqBitLength = 10;
    idgen::IdGenerator::CreateInstance(options);
}

void test_multithread_run_create_instance()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; i++)
    {
        threads.emplace_back(create_instance_fun);
    }
    for (auto &t : threads)
    {
        t.join();
    }
}

int main()
{
    if (multiThread) {
        test_multithread_run_create_instance(); // 测试多线程调用同时初始化，也确保只会安全初始化一次，确保单例唯一性
    } else {
        create_instance_fun(); // 单线程调用初始化
    }

    if (multiThread)
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < threadCount; i++)
        {
            threads.emplace_back(RunMultiThread);
        }
        for (auto &t : threads)
        {
            t.join();
        }
    }
    else
    {
        RunSingle();
    }
}
