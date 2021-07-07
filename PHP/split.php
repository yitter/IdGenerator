<?php

declare(strict_types=1);

for ($i = 0; $i < 50000; $i++) {
    $id = \SnowDrift::NextId();
    $seq = $id & ((1 << (int)ini_get('snowdrift.SeqBitLength')) - 1);
    $id = $id >> (int)ini_get('snowdrift.SeqBitLength');
    $wid = $id & ((1 << (int)ini_get('snowdrift.WorkerIdBitLength')) - 1);
    $id = $id >> (int)ini_get('snowdrift.WorkerIdBitLength');
    $time = $id;
    echo "time:$time, wid:$wid, seq:$seq" . PHP_EOL;
}
