/*
 * 版权属于：yitter(yitter@126.com)
 * 翻译为C++代码：fuzhufang(fuzhufang@126.com)
 * 开源地址：https://github.com/yitter/idgenerator
 */

#pragma once
#include <assert.h>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <thread>
#include <stdexcept>
// #include <sys/time.h>

namespace idgen
{
    typedef struct IdGenOptions
    {
        /// 雪花计算方法,（1-漂移算法|2-传统算法），默认1
        uint8_t Method{1};

        /// 基础时间（ms单位），不能超过当前系统时间
        uint64_t BaseTime{1582136402000};

        /// 机器码，必须由外部设定，最大值 2^WorkerIdBitLength-1
        uint32_t WorkerId{0};

        /// 机器码位长，默认值6，取值范围 [1, 15]（要求：序列数位长+机器码位长不超过22）
        uint8_t WorkerIdBitLength{6};

        /// 序列数位长，默认值6，取值范围 [3, 21]（要求：序列数位长+机器码位长不超过22）
        uint8_t SeqBitLength{6};

        /// 最大序列数（含），设置范围 [MinSeqNumber, 2^SeqBitLength-1]，默认值0，表示最大序列数取最大值（2^SeqBitLength-1]）
        uint32_t MaxSeqNumber{0};

        /// 最小序列数（含），默认值5，取值范围 [5, MaxSeqNumber]，每毫秒的前5个序列数对应编号0-4是保留位，其中1-4是时间回拨相应预留位，0是手工新值预留位
        uint32_t MinSeqNumber{5};

        /// 最大漂移次数（含），默认2000，推荐范围 500-20000（与计算能力有关）
        uint32_t TopOverCostCount{2000};

    } IdGeneratorOptions;

    typedef struct SnowFlakeWorker
    {
        /// 雪花计算方法,（1-漂移算法|2-传统算法），默认1
        uint8_t Method{0};

        /// 基础时间
        uint64_t BaseTime{0};

        /// 机器码
        uint32_t WorkerId{0};

        /// 机器码位长
        uint8_t WorkerIdBitLength{0};

        /// 自增序列数位长
        uint8_t SeqBitLength{0};

        /// 最大序列数（含）
        uint32_t MaxSeqNumber{0};

        /// 最小序列数（含）
        uint32_t MinSeqNumber{0};

        /// 最大漂移次数（含）
        uint32_t TopOverCostCount{0};

        uint8_t _TimestampShift{0};
        uint32_t _CurrentSeqNumber{0};
        int64_t _LastTimeTick{0};
        int64_t _TurnBackTimeTick{0};
        uint8_t _TurnBackIndex{0};
        bool _IsOverCost{false};
        uint32_t _OverCostCountInOneTerm{0};
        uint32_t _GenCountInOneTerm{0};
        uint32_t _TermIndex{0};

    } SnowFlakeWorker;

    inline static int64_t GetCurrentTime()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    inline static int64_t GetCurrentMicroTime()
    {
        using namespace std::chrono;
        return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    }

    class SnowWorker
    {
    public:
        int64_t GetCurrentTimeTick()
        {
            return GetCurrentTime() - (int64_t)_SnowFlakeWorker.BaseTime;
        }

        int64_t GetNextTimeTick()
        {
            int64_t tempTimeTicker = GetCurrentTimeTick();
            while (tempTimeTicker <= _SnowFlakeWorker._LastTimeTick)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 暂停1ms
                tempTimeTicker = GetCurrentTimeTick();
            }
            return tempTimeTicker;
        }

        void EndOverCostAction(int64_t useTimeTick)
        {
            // if (_SnowFlakeWorker._TermIndex > 10000) {
            //     _SnowFlakeWorker._TermIndex = 0;
            // }
        }

        int64_t NextOverCostId()
        {
            int64_t currentTimeTick = GetCurrentTimeTick();
            if (currentTimeTick > _SnowFlakeWorker._LastTimeTick)
            {
                EndOverCostAction(currentTimeTick);
                _SnowFlakeWorker._LastTimeTick = currentTimeTick;
                _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;
                _SnowFlakeWorker._IsOverCost = false;
                _SnowFlakeWorker._OverCostCountInOneTerm = 0;
                _SnowFlakeWorker._GenCountInOneTerm = 0;
                return CalcId();
            }
            if (_SnowFlakeWorker._OverCostCountInOneTerm > _SnowFlakeWorker.TopOverCostCount)
            {
                EndOverCostAction(currentTimeTick);
                _SnowFlakeWorker._LastTimeTick = GetNextTimeTick();
                _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;
                _SnowFlakeWorker._IsOverCost = false;
                _SnowFlakeWorker._OverCostCountInOneTerm = 0;
                _SnowFlakeWorker._GenCountInOneTerm = 0;
                return CalcId();
            }
            if (_SnowFlakeWorker._CurrentSeqNumber > _SnowFlakeWorker.MaxSeqNumber)
            {
                _SnowFlakeWorker._LastTimeTick++;
                _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;
                _SnowFlakeWorker._IsOverCost = true;
                _SnowFlakeWorker._OverCostCountInOneTerm++;
                _SnowFlakeWorker._GenCountInOneTerm++;
                return CalcId();
            }

            _SnowFlakeWorker._GenCountInOneTerm++;
            return CalcId();
        }

        int64_t NextNormalId()
        {
            int64_t currentTimeTick = GetCurrentTimeTick();
            if (currentTimeTick < _SnowFlakeWorker._LastTimeTick)
            {
                if (_SnowFlakeWorker._TurnBackTimeTick < 1)
                {
                    _SnowFlakeWorker._TurnBackTimeTick = _SnowFlakeWorker._LastTimeTick - 1;
                    _SnowFlakeWorker._TurnBackIndex++;
                    // 每毫秒序列数的前 5 位是预留位，0 用于手工新值，1-4 是时间回拨次序
                    // 支持 4 次回拨次序（避免回拨重叠导致 ID 重复），可无限次回拨（次序循环使用）。
                    if (_SnowFlakeWorker._TurnBackIndex > 4)
                    {
                        _SnowFlakeWorker._TurnBackIndex = 1;
                    }
                }

                // std::this_thread::sleep_for(std::chrono::milliseconds(1));;  // 暂停1ms
                return CalcTurnBackId();
            }

            if (_SnowFlakeWorker._TurnBackTimeTick > 0)
            {
                _SnowFlakeWorker._TurnBackTimeTick = 0;
            }

            if (currentTimeTick > _SnowFlakeWorker._LastTimeTick)
            {
                _SnowFlakeWorker._LastTimeTick = currentTimeTick;
                _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;
                return CalcId();
            }

            if (_SnowFlakeWorker._CurrentSeqNumber > _SnowFlakeWorker.MaxSeqNumber)
            {
                _SnowFlakeWorker._TermIndex++;
                _SnowFlakeWorker._LastTimeTick++;
                _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;
                _SnowFlakeWorker._IsOverCost = true;
                _SnowFlakeWorker._OverCostCountInOneTerm = 1;
                _SnowFlakeWorker._GenCountInOneTerm = 1;
                return CalcId();
            }

            return CalcId();
        }

        int64_t CalcId()
        {
            uint64_t result = (_SnowFlakeWorker._LastTimeTick << _SnowFlakeWorker._TimestampShift) | (_SnowFlakeWorker.WorkerId << _SnowFlakeWorker.SeqBitLength) |
                              (_SnowFlakeWorker._CurrentSeqNumber);
            _SnowFlakeWorker._CurrentSeqNumber++;
            return result;
        }

        int64_t CalcTurnBackId()
        {
            uint64_t result = (_SnowFlakeWorker._LastTimeTick << _SnowFlakeWorker._TimestampShift) | (_SnowFlakeWorker.WorkerId << _SnowFlakeWorker.SeqBitLength) |
                              (_SnowFlakeWorker._TurnBackIndex);
            _SnowFlakeWorker._TurnBackTimeTick--;
            return result;
        }

        int64_t WorkerM1NextId()
        {
            std::lock_guard<std::mutex> lock(_ThreadMutex);
            int64_t id = _SnowFlakeWorker._IsOverCost ? NextOverCostId() : NextNormalId();
            return id;
        }

        int64_t WorkerM2NextId()
        {
            std::lock_guard<std::mutex> lock(_ThreadMutex);

            int64_t currentTimeTick = GetCurrentTimeTick();
            if (_SnowFlakeWorker._LastTimeTick == currentTimeTick)
            {
                _SnowFlakeWorker._CurrentSeqNumber = (++_SnowFlakeWorker._CurrentSeqNumber) & _SnowFlakeWorker.MaxSeqNumber;
                if (_SnowFlakeWorker._CurrentSeqNumber == 0)
                {
                    currentTimeTick = GetNextTimeTick();
                }
            }
            else
            {
                _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;
            }

            _SnowFlakeWorker._LastTimeTick = currentTimeTick;
            int64_t id = (int64_t)((currentTimeTick << _SnowFlakeWorker._TimestampShift) |
                                   (_SnowFlakeWorker.WorkerId << _SnowFlakeWorker.SeqBitLength) |
                                   _SnowFlakeWorker._CurrentSeqNumber);

            return id;
        }

        void SetOptions(IdGeneratorOptions options)
        {
            // 1.BaseTime
            if (options.BaseTime == 0)
            {
                _SnowFlakeWorker.BaseTime = 1582136402000;
            }
            else if (options.BaseTime < 631123200000 || (int64_t)options.BaseTime > GetCurrentTime())
            {
                throw std::invalid_argument("BaseTime error.");
            }
            else
            {
                _SnowFlakeWorker.BaseTime = options.BaseTime;
            }

            // 2.WorkerIdBitLength
            if (options.WorkerIdBitLength <= 0)
            {
                throw std::invalid_argument("WorkerIdBitLength error.(range:[1, 21])");
            }
            if (options.SeqBitLength + options.WorkerIdBitLength > 22)
            {
                throw std::invalid_argument("error：WorkerIdBitLength + SeqBitLength <= 22");
            }
            else
            {
                // _SnowFlakeWorker.WorkerIdBitLength = options.WorkerIdBitLength;
                _SnowFlakeWorker.WorkerIdBitLength = options.WorkerIdBitLength <= 0 ? 6 : options.WorkerIdBitLength;
            }

            // 3.WorkerId
            uint32_t maxWorkerIdNumber = (1 << options.WorkerIdBitLength) - 1;
            if (maxWorkerIdNumber == 0)
            {
                maxWorkerIdNumber = 63;
            }
            if (options.WorkerId > maxWorkerIdNumber)
            {
                throw std::invalid_argument("WorkerId error. (range:[0, {2^options.WorkerIdBitLength-1]}");
            }
            else
            {
                _SnowFlakeWorker.WorkerId = options.WorkerId;
            }

            // 4.SeqBitLength
            if (options.SeqBitLength < 2 || options.SeqBitLength > 21)
            {
                throw std::invalid_argument("SeqBitLength error. (range:[2, 21])");
            }
            else
            {
                _SnowFlakeWorker.SeqBitLength = options.SeqBitLength <= 0 ? 6 : options.SeqBitLength;
            }

            // 5.MaxSeqNumber
            uint32_t maxSeqNumber = (1 << options.SeqBitLength) - 1;
            if (maxSeqNumber == 0)
            {
                maxSeqNumber = 63;
            }
            if (options.MaxSeqNumber > maxSeqNumber)
            {
                throw std::invalid_argument("MaxSeqNumber error. (range:[1, {2^options.SeqBitLength-1}]");
            }
            else
            {
                _SnowFlakeWorker.MaxSeqNumber = options.MaxSeqNumber <= 0 ? maxSeqNumber : options.MaxSeqNumber;
            }

            // 6.MinSeqNumber
            if (options.MinSeqNumber < 5 || options.MinSeqNumber > maxSeqNumber)
            {
                throw std::invalid_argument("MinSeqNumber error. (range:[5, {options.MinSeqNumber}]");
            }
            else
            {
                _SnowFlakeWorker.MinSeqNumber = options.MinSeqNumber <= 0 ? 5 : options.MinSeqNumber;
            }

            // 7.TopOverCostCount
            if (options.TopOverCostCount > 10000)
            {
                throw std::invalid_argument("TopOverCostCount error. (range:[0, 10000]");
            }
            else
            {
                //_SnowFlakeWorker.TopOverCostCount = options.TopOverCostCount <= 0 ? 2000 : options.TopOverCostCount;
                _SnowFlakeWorker.TopOverCostCount = options.TopOverCostCount;
            }

            // 8.Others
            _SnowFlakeWorker._TimestampShift = _SnowFlakeWorker.WorkerIdBitLength + _SnowFlakeWorker.SeqBitLength;
            _SnowFlakeWorker._CurrentSeqNumber = _SnowFlakeWorker.MinSeqNumber;

            _SnowFlakeWorker.Method = options.Method;
            if (options.Method == 2)
            {
                this->_NextId = &SnowWorker::WorkerM2NextId;
            }
            else
            {
                this->_NextId = &SnowWorker::WorkerM1NextId;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                ; // 暂停500ms
            }
        }

        inline int64_t NextId()
        {
            return (this->*_NextId)();
        }

    protected:
        std::mutex _ThreadMutex;
        SnowFlakeWorker _SnowFlakeWorker;
        int64_t (SnowWorker::*_NextId)();
    };

    class IdGenerator
    {
    public:
        // 禁止拷贝构造函数和赋值操作符
        IdGenerator(const IdGenerator &) = delete;
        IdGenerator &operator=(const IdGenerator &) = delete;

        static void CreateInstance(uint32_t WorkerId)
        {
            IdGeneratorOptions options;
            options.WorkerId = WorkerId;
            CreateInstance(options);
        }

        static void CreateInstance(IdGeneratorOptions options)
        {
            static std::once_flag initInstanceFlag;
            // 使用std::once_flag和std::call_once保证只初始化一次
            std::call_once(initInstanceFlag, [options]()
                           { createInstance(options); });
        }

        static int64_t NextId()
        {
            assert(instance && "Please call CreateInstance first to create an instance");
            return instance->_sworker.NextId();
        }

    private:
        IdGenerator()
        {
        }

        // 使用std::once_flag和std::call_once保证只初始化一次
        static void createInstance(IdGeneratorOptions options)
        {
            static IdGenerator obj;
            // 理论上保证了这个函数只会被调用一次，就算是多线程并发调用也不例外
            assert(nullptr == instance && "Theoretically, it is guaranteed that this function will only be called once, even if it is called concurrently by multiple threads.");
            instance = &obj;
            instance->_sworker.SetOptions(options);
            // printf("调用 %s \n", __FUNCTION__); // 在多个线程调用 CreateInstance 时候，看是否出现打印多次
        }

    private:
        SnowWorker _sworker;

    private:
        static IdGenerator *instance;
    };

    IdGenerator *IdGenerator::instance = nullptr;

} // namespace idgen
