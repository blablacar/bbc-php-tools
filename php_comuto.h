/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 321634 2012-01-01 13:15:04Z felipe $ */

#ifndef PHP_COMUTO_H
#define PHP_COMUTO_H

#include "php.h"

extern zend_module_entry comuto_module_entry;
#define phpext_comuto_ptr &comuto_module_entry

#ifdef PHP_WIN32
#	define PHP_COMUTO_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_COMUTO_API __attribute__ ((visibility("default")))
#else
#	define PHP_COMUTO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define COM_ARRAY_RAND_TYPE_STRING 1
#define COM_ARRAY_RAND_TYPE_INT    2
#define RANDOM_STRING_SIZE 42 /* yes */

#define GET_RANDOM_NUMBER(_number, _min, _max) {(_number) = php_rand(TSRMLS_C); RAND_RANGE((_number), (_min), (_max), PHP_RAND_MAX);}
#define generate_random_long(_long) (GET_RANDOM_NUMBER((_long), 0, INT_MAX))

#define REPLACE_FUNCTION_HANDLE(funct) do { \
		zend_function *__funct = NULL; \
		if(zend_hash_find(EG(function_table), #funct, sizeof(#funct), (void **)&__funct) == SUCCESS) { \
		__funct->internal_function.handler = COM_FN(funct); } \
} while (0)

#define COM_FUNCTION(funct) PHP_FUNCTION(com_##funct)
#define COM_FE(funct, arginfo) PHP_FE(com_##funct, arginfo)
#define COM_FN(funct) PHP_FN(com_##funct)

PHP_MINIT_FUNCTION(comuto);
PHP_MSHUTDOWN_FUNCTION(comuto);
PHP_RINIT_FUNCTION(comuto);
PHP_RSHUTDOWN_FUNCTION(comuto);
PHP_MINFO_FUNCTION(comuto);

COM_FUNCTION(array_create_rand);
COM_FUNCTION(array_stats);
COM_FUNCTION(get_var_memory_usage);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(comuto)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(comuto)
*/

/* In every utility function you add that needs to use variables 
   in php_comuto_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as COMUTO_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define COMUTO_G(v) TSRMG(comuto_globals_id, zend_comuto_globals *, v)
#else
#define COMUTO_G(v) (comuto_globals.v)
#endif

#endif	/* PHP_COMUTO_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
