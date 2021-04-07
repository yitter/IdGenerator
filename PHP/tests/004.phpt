--TEST--
Check for snowdrift batch get unique
--SKIPIF--
<?php if (!extension_loaded("snowdrift")) print "skip"; ?>
--FILE--
<?php
$arr = [];
$max = 100000;
foreach (SnowDrift::NextNumId($max) as $id) {
	$arr[$id] = '';
}
var_dump(count($arr));
?>
--EXPECT--
int(100000)