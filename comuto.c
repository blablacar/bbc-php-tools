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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_rand.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_comuto.h"

/* If you declare any globals in php_comuto.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(comuto)
*/

/* True global resources - no need for thread safety here */
static int le_comuto;

ZEND_BEGIN_ARG_INFO(arginfo_array_create_rand, 0)
	ZEND_ARG_INFO(0, num_item)
ZEND_END_ARG_INFO()
/* {{{ comuto_functions[]
 *
 * Every user visible function must have an entry in comuto_functions[].
 */
const zend_function_entry comuto_functions[] = {
	PHP_FE(array_create_rand,	arginfo_array_create_rand)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in comuto_functions[] */
};
/* }}} */

/* {{{ comuto_module_entry
 */
zend_module_entry comuto_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"comuto",
	comuto_functions,
	PHP_MINIT(comuto),
	PHP_MSHUTDOWN(comuto),
	PHP_RINIT(comuto),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(comuto),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(comuto),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_COMUTO
ZEND_GET_MODULE(comuto)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("comuto.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_comuto_globals, comuto_globals)
    STD_PHP_INI_ENTRY("comuto.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_comuto_globals, comuto_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_comuto_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_comuto_init_globals(zend_comuto_globals *comuto_globals)
{
	comuto_globals->global_value = 0;
	comuto_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(comuto)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(comuto)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(comuto)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(comuto)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(comuto)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "comuto support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


PHP_FUNCTION(array_create_rand)
{
	long num_items;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &num_items) == FAILURE) {
		return;
	}

	if(num_items <= 0) {
		php_error_docref(NULL, E_WARNING, "Size should be a positive integer");
		return;
	}

	HashTable *table;
	array_init_size(return_value, num_items);

	ulong i              = 0;
	char alpha[]         = "0123456789abcdefghijklmnopqrstuvwxyz";
	long str_size, index = 0;
	char *str            = NULL;
	size_t j             = 0;

	for(; i<num_items; i++, j=0) {
		str_size = php_rand(TSRMLS_C); RAND_RANGE(str_size, 1, 64, PHP_RAND_MAX);
		str      = emalloc(str_size + 1);

		while(str_size--){
			index = php_rand(TSRMLS_C); RAND_RANGE(index, 1, sizeof(alpha) - 2, PHP_RAND_MAX);
			str[j++] = alpha[index];
		}
		str[j] = '\0';
		add_next_index_string(return_value, str, 0);
	}
}


/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
