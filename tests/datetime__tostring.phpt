--TEST--
Test comuto tools added a __toString() to DateTime
--INI--
date.timezone = UTC
--FILE--
<?php
$r = new ReflectionClass('DateTime');
var_dump($r->hasMethod('__toString'));
echo new DateTime();
?>
--EXPECTF--
bool(true)
%sT%s