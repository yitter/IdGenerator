#include <stdint.h>

typedef struct snowflake
{
  uint8_t Method;
  uint64_t BaseTime;
  uint16_t WorkerId;
  uint8_t WorkerIdBitLength;
  uint8_t SeqBitLength;
  uint32_t MaxSeqNumber;
  uint32_t MinSeqNumber;
  uint32_t TopOverCostCount;

  uint8_t _TimestampShift;
  uint32_t _CurrentSeqNumber;
  int64_t _LastTimeTick;
  int64_t _TurnBackTimeTick;
  uint8_t _TurnBackIndex;
  uint8_t _IsOverCost;
  uint32_t _OverCostCountInOneTerm;
  // uint32_t _GenCountInOneTerm;
  // uint32_t _TermIndex;
  volatile unsigned int _Lock;
} snowflake;

void Config(snowflake *flake);
uint64_t NextId(snowflake *flake);
uint64_t *NextNumId(snowflake *flake, uint32_t num);