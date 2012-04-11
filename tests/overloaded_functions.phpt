--TEST--
Test overloaded functions
--INI--
comuto.override_php_ini_settings=1
--FILE--
<?php
ini_set('foo', 'bar');
ini_get('bar');
ini_restore('baz');
get_cfg_var('bar');
ini_restore('html_errors');
ini_set('html_errors', 'off');
ini_restore('html_errors');
?>
--EXPECTF--
Notice: ini value foo doesn't exist in %s on line %d

Notice: ini value bar doesn't exist in %s on line %d

Notice: ini value baz doesn't exist in %s on line %d

Notice: Configuration value bar doesn't exist in %s on line %d

Notice: ini value html_errors has never been changed or it's already been restored, no need to restore in %s on line %d