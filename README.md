# Comuto Tools - PHP tools used at covoiturage.fr

This extension is part of our R&D department.
It's aimed to be used in our environnement, we don't provide
any support at all.

It is subject to huge changes with no future disclamer.
Please, read the licence warnings for more info.

For more info about what is provided inside,
please see the tests.
Some doc may be written in future.

## Installation
```bash
cd comuto/
phpize
./configure
make
make install
```

## Examples

### comuto_array_rand()

``comuto_array_rand(int $num_of_items, int $type = COMUTO_ARRAY_RAND_TYPE_STRING)``

```php
<?php
var_dump(comuto_array_create_rand(2));
/*array(2) {
  [0]=>
  string(18) "vvcpi4wl7qvbz5jeu4"
  [1]=>
  string(4) "54mb"
}*/

var_dump(comuto_array_create_rand(4, COMUTO_ARRAY_RAND_TYPE_INT));
/*array(4) {
  [0]=>
  int(539487114)
  [1]=>
  int(2103125655)
  [2]=>
  int(281659599)
  [3]=>
  int(1277834078)
}*/

var_dump(comuto_array_create_rand(3, COMUTO_ARRAY_RAND_TYPE_STRING | COMUTO_ARRAY_RAND_TYPE_INT));
/*array(3) {
  [0]=>
  string(26) "vmjskl4lhabrnlwqs1nun4g85t"
  [1]=>
  int(272665550)
  [2]=>
  string(35) "qlgaezuhlbqv3ehy49zr4mujtyc543yuof4"
}*/
```

### DateTime::__toString()

``DateTime::__toString()``

```php
<?php
date_default_timezone_set('UTC');

$d = new DateTime;
echo $d; /* 2012-04-13T09:45:07+0000 */

ini_set('comuto.datetime_defaut_format', 'd-m-Y');
echo $d; /* 13-04-2012 */
```

### comuto_array_stats()

``comuto_array_stats(array $array)``

```php
<?php
$array = array('foo', 'bar', 4 => array(str_repeat('a', 1024), 123), new StdClass);
var_dump(comuto_array_stats($array));

/*
array(6) {
  ["array_size"]=>
  int(8)
  ["array_struct_memory_consumption"]=>
  int(168)
  ["array_memory_consumption"]=>
  int(2366)
  ["refcount"]=>
  int(1)
  ["persistent_alloc"]=>
  bool(false)
  ["internal_ll_stats"]=>
  array(8) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(0)
    [3]=>
    int(0)
    [4]=>
    int(1)
    [5]=>
    int(1)
    [6]=>
    int(0)
    [7]=>
    int(0)
  }
}*/
```

### comuto_get_var_memory_usage()

``comuto_get_var_memory_usage($somevar)``

```php
<?php
$int = 123;
$str = "foo bar and baz (why not ?)";
$obj = new SplObjectStorage();
$array = array('foo', 'bar', $str, array($obj, $str, $str), $int, 'foo'=> array($int));
$complexarray = array($array, array(array(array('foo', new DateTime, array()))));

echo comuto_get_var_memory_usage($int); /* 32 */
echo comuto_get_var_memory_usage($str); /* 59 */
echo comuto_get_var_memory_usage($obj); /* 872 */
echo comuto_get_var_memory_usage($array); /* 1561 */
echo comuto_get_var_memory_usage($array); /* 3308 */
```