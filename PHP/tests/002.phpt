--TEST--
Check for snowdrift unique
--SKIPIF--
<?php if (!extension_loaded("snowdrift")) print "skip"; ?>
--FILE--
<?php
$arr = [];
$max = 100000;
for ($i = 0; $i < $max; $i++) {
	$id = SnowDrift::NextId();
	$arr[$id] = '';
}
var_dump(count($arr));
?>
--EXPECT--
int(100000)