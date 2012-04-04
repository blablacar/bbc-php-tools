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
  | Author: Julien PAULI (jpauli@php.net) - Copyright (c) 2012           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_COMUTO_H
#define PHP_COMUTO_H

#include "php.h"
#include "comuto.h"

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

#define REPLACE_FUNCTION_HANDLE(funct) do { \
		zend_function *__funct = NULL; \
		if(zend_hash_find(EG(function_table), #funct, sizeof(#funct), (void **)&__funct) == SUCCESS) { \
		__funct->internal_function.handler = COM_FN(funct); } \
} while (0)

#define COM_FUNCTION(funct) PHP_FUNCTION(comuto_##funct)
#define COM_FE(funct, arginfo) PHP_FE(comuto_##funct, arginfo)
#define COM_FN(funct) PHP_FN(comuto_##funct)

#define CLEAR_HASHTABLE(ht) do { zend_hash_destroy(ht); FREE_HASHTABLE(ht); } while(0)

static size_t get_var_memory_usage_ex(zval *val, HashTable *zval_cache);
static void generate_random_string(char **str);

PHP_MINIT_FUNCTION(comuto);
PHP_MSHUTDOWN_FUNCTION(comuto);
PHP_RINIT_FUNCTION(comuto);
PHP_RSHUTDOWN_FUNCTION(comuto);
PHP_MINFO_FUNCTION(comuto);

COM_FUNCTION(array_create_rand);
COM_FUNCTION(array_stats);
COM_FUNCTION(get_var_memory_usage);


ZEND_BEGIN_MODULE_GLOBALS(comuto)
	zend_bool override_ini_settings;
	char *datetime_defaut_format;
ZEND_END_MODULE_GLOBALS(comuto)

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
