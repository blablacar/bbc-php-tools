--TEST--
Check for comuto presence
--SKIPIF--
<?php if (!extension_loaded("comuto_tools")) print "skip"; ?>
--FILE--
<?php
echo "comuto extension is available";
?>
--EXPECT--
comuto extension is available
