--TEST--
Test com_array_create_rand() output
--FILE--
<?php
var_dump(com_array_create_rand(2, COM_ARRAY_RAND_TYPE_STRING));
var_dump(com_array_create_rand(2, COM_ARRAY_RAND_TYPE_INT));
var_dump(com_array_create_rand(2, 42));
var_dump(com_array_create_rand(-30));
?>
--EXPECTF--
array(2) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
}
array(2) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
}

Warning: com_array_create_rand(): Unknown type in %s on line %d
array(0) {
}

Warning: com_array_create_rand(): Size should be a positive integer in %s on line %d
NULL