--TEST--
Test comuto tools added a __toString() to DateTime
--FILE--
<?php
$r = new ReflectionClass('DateTime');
var_dump($r->hasMethod('__toString'));
?>
--EXPECTF--
bool(true)