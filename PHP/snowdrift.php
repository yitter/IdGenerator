<?php

declare(strict_types=1);

namespace SnowDrift;

use FFI;
use FFI\CData;

final class SnowFlake
{

    private FFI $ffi;
    private CData $flake;
    private CData $pflake;

    public function __construct(array $config = ['Method' => 1, 'BaseTime' => 0, 'WorkerId' => 1, 'WorkerIdBitLength' => 6, 'SeqBitLength' => 10, 'TopOverCostCount' => 2000])
    {
        $this->ffi = FFI::cdef(file_get_contents($this->getHeaders()), $this->getLibrary());
        $this->flake = $this->ffi->new("struct snowflake");
        foreach ($config as $name => $val) {
            $this->flake->$name = $val;
        }
        $this->pflake = FFI::addr($this->flake);
        $this->ffi->Config($this->pflake);
    }

    public function getFFI(): FFI
    {
        return $this->ffi;
    }

    public function getFlake(): CData
    {
        return $this->flake;
    }

    public function getPflake(): CData
    {
        return $this->pflake;
    }

    public function nextId(): int
    {
        return $this->ffi->NextId($this->pflake);
    }

    public function getHeaders(): string
    {
        return __DIR__ . '/src/snowflake/snowflake.h';
    }

    public function getLibrary(): ?string
    {
        return __DIR__ . '/src/snowflake/libsnow.so';
    }
}

$total = 50000;

$snowflake = new SnowFlake(['Method' => 1, 'BaseTime' => 1577808000000, 'WorkerId' => 1, 'WorkerIdBitLength' => 1, 'SeqBitLength' => 10, 'TopOverCostCount' => 2000]);
$ffi = $snowflake->getFFI();
$pflake = $snowflake->getPflake();

// $res = [];
$start = microtime(true);
for ($i = 0; $i < $total; $i++) {
    // $res[] = \SnowDrift::NextId();
    \SnowDrift::NextId(2);
}
echo sprintf("扩展漂移算法,PHP循环获取：%d，%s ms", $total, ((microtime(true) - $start)) * 1000) . PHP_EOL;

// $res = [];
$start = microtime(true);
foreach (\SnowDrift::NextNumId($total) as $val) {
    // $res[] = $val;
}
echo sprintf("扩展漂移算法,C循环获取：%d，%s ms", $total, ((microtime(true) - $start)) * 1000) . PHP_EOL;

// $res = [];
$start = microtime(true);
for ($i = 0; $i < $total; $i++) {
    // $res[] = $ffi->NextId($pflake);
    $ffi->NextId($pflake);
}
echo sprintf("FFI漂移算法,PHP循环获取：%d，%s ms", $total, ((microtime(true) - $start)) * 1000) . PHP_EOL;

// $res = [];
$start = microtime(true);
$tmp = $ffi->NextNumId($pflake, $total);
for ($i = 0; $i < $total; $i++) {
    // $res[] = $tmp[$i];
}
echo sprintf("FFI漂移算法,C循环获取：%d，%s ms", $total, ((microtime(true) - $start)) * 1000) . PHP_EOL;
