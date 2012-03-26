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
	PHP_FE(com_array_create_rand,	arginfo_array_create_rand)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in comuto_functions[] */
};
/* }}} */

/* {{{ comuto_module_entry
 */
zend_module_entry comuto_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"comuto_tools",
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
	REGISTER_LONG_CONSTANT("COM_ARRAY_RAND_TYPE_STRING", COM_ARRAY_RAND_TYPE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COM_ARRAY_RAND_TYPE_INT", COM_ARRAY_RAND_TYPE_INT, CONST_CS | CONST_PERSISTENT);
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
	php_info_print_table_header(2, "Comuto Tools support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

static void generate_random_string(char **str)
{
	long str_size, index;
	static const char alpha[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	size_t j                  = 0;
	GET_RANDOM_NUMBER(str_size, 1, RANDOM_STRING_SIZE);
	*str      = emalloc(str_size + 1);

	while(str_size--){
		GET_RANDOM_NUMBER(index, 1, sizeof(alpha) - 2);
		(*str)[j++] = alpha[index];
	}
	(*str)[j] = '\0';
}

PHP_FUNCTION(com_array_create_rand)
{
	long num_items, items_type;
	items_type = COM_ARRAY_RAND_TYPE_STRING;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &num_items, &items_type) == FAILURE) {
		return;
	}

	if(num_items <= 0) {
		php_error_docref(NULL, E_WARNING, "Size should be a positive integer");
		return;
	}

	HashTable *table;
	array_init_size(return_value, num_items);

	ulong i;
	long index;
	long generated_long;
	char *generated_string = NULL;

	for(i=0; i<num_items; i++) {
		switch (items_type) {
			case (COM_ARRAY_RAND_TYPE_STRING | COM_ARRAY_RAND_TYPE_INT):
				GET_RANDOM_NUMBER(index, 0, 1);
				if(index) {
					generate_random_string(&generated_string);
					add_next_index_string(return_value, generated_string, 0);
				} else {
					generate_random_long(generated_long);
					add_next_index_long(return_value, generated_long);
				}
			break;
			case COM_ARRAY_RAND_TYPE_STRING:
				generate_random_string(&generated_string);
				add_next_index_string(return_value, generated_string, 0);
			break;
			case COM_ARRAY_RAND_TYPE_INT:
				generate_random_long(generated_long);
				add_next_index_long(return_value, generated_long);
			break;
			default:
				php_error_docref(NULL, E_WARNING, "Unknown type");
				return;
		}
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
