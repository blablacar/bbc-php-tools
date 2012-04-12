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

/**
 * Ideas to be added :
 * - get_class_constants()
 * - get_declared_objects()
 * - json_to_xml()
 * - array_stats()
 * - mmap shm as a resource
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_rand.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_comuto.h"
#include "comuto_ov_functions.h"
#include "Zend/zend_interfaces.h"

/* ---------------------- Ext Head -------------------- */

ZEND_DECLARE_MODULE_GLOBALS(comuto)

static void php_comuto_init_globals(zend_comuto_globals *comuto_globals)
{
	comuto_globals->override_ini_settings = 0;
	comuto_globals->datetime_defaut_format = NULL;
}

static int le_comuto;

ZEND_BEGIN_ARG_INFO_EX(arginfo_comuto_array_create_rand, 0, 0, 1)
	ZEND_ARG_INFO(0, size)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(arginfo_comuto_array_stats, 0)
	ZEND_ARG_ARRAY_INFO(0, array, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(arginfo_comuto_get_var_memory_usage, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

static const zend_function_entry comuto_functions[] = {
	COM_FE(array_create_rand,    arginfo_comuto_array_create_rand)
	COM_FE(get_var_memory_usage, arginfo_comuto_get_var_memory_usage)
	COM_FE(array_stats,          arginfo_comuto_array_stats)
	PHP_FE_END
};

static const zend_function_entry datetime_functions[] = {
	PHP_ME(DateTime, __toString, NULL, ZEND_ACC_PUBLIC)
};

static const zend_module_dep comuto_deps[] = {
		ZEND_MOD_REQUIRED("date")
		ZEND_MOD_END
};

zend_module_entry comuto_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	comuto_deps,
	"comuto_tools",
	comuto_functions,
	PHP_MINIT(comuto),
	PHP_MSHUTDOWN(comuto),
	PHP_RINIT(comuto),
	PHP_RSHUTDOWN(comuto),
	PHP_MINFO(comuto),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_COMUTO
ZEND_GET_MODULE(comuto)
#endif

PHP_INI_BEGIN()
    STD_PHP_INI_BOOLEAN("comuto.override_php_ini_settings",      "yes", PHP_INI_SYSTEM, OnUpdateBool, override_ini_settings, zend_comuto_globals, comuto_globals)
    STD_PHP_INI_ENTRY("comuto.datetime_defaut_format", "Y-m-d\\TH:i:sO", PHP_INI_ALL, OnUpdateString, datetime_defaut_format, zend_comuto_globals, comuto_globals)
PHP_INI_END()


/* ---------------------- Ext Hooks -------------------- */

PHP_MINIT_FUNCTION(comuto)
{
	zend_class_entry **datetime = NULL;
	if (zend_hash_find(CG(class_table), "datetime", sizeof("datetime"), (void**)&datetime) == SUCCESS) {
		zend_internal_function tostring;
		zend_function *reg_tostring = NULL;
		char *function_name_lc = NULL;
		tostring.handler = datetime_functions->handler;
		tostring.type = ZEND_INTERNAL_FUNCTION;
		tostring.module = &comuto_module_entry;
		tostring.function_name = (char *)datetime_functions->fname;
		tostring.scope = *datetime;
		tostring.prototype = NULL;
		tostring.arg_info = NULL;
		tostring.num_args = 0;
		tostring.required_num_args = 0;
		tostring.pass_rest_by_reference = 0;
		tostring.return_reference = 0;
		tostring.fn_flags = datetime_functions->flags;
		function_name_lc = zend_str_tolower_dup(tostring.function_name, strlen(tostring.function_name));
		zend_hash_add(&(*datetime)->function_table,
					  function_name_lc,
					  strlen(tostring.function_name)+1,
					  &tostring,
					  sizeof(zend_function),
					  (void **)&reg_tostring);
		(*datetime)->__tostring = reg_tostring;
		efree(function_name_lc);
	} else {
		return FAILURE;
	}

	REGISTER_LONG_CONSTANT("COMUTO_ARRAY_RAND_TYPE_STRING", COMUTO_ARRAY_RAND_TYPE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COMUTO_ARRAY_RAND_TYPE_INT", COMUTO_ARRAY_RAND_TYPE_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(comuto)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_RINIT_FUNCTION(comuto)
{
	if(INI_BOOL("comuto.override_php_ini_settings")) {
		REPLACE_FUNCTION_HANDLE(ini_get);
		REPLACE_FUNCTION_HANDLE(ini_set);
		REPLACE_FUNCTION_HANDLE(ini_restore);
		REPLACE_FUNCTION_HANDLE(get_cfg_var);
	}
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(comuto)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(comuto)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Comuto Tools support", "enabled");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}

/* ---------------------- Ext Body -------------------- */

PHP_METHOD(DateTime, __toString)
{
	zend_function *function =  NULL;
	zval *param = NULL, *retval = NULL;

	HashTable function_table = Z_OBJCE_P(getThis())->function_table;
	zend_hash_find(&function_table, "format", sizeof("format"), (void **)&function);

	ALLOC_INIT_ZVAL(param);
	ZVAL_STRING(param, COMUTO_G(datetime_defaut_format), 1);

	zend_call_method_with_1_params(&getThis(),  Z_OBJCE_P(getThis()), &function, "format", &retval, param);

	zval_ptr_dtor(&param);

	RETURN_ZVAL(retval, 1, 1)
}

COM_FUNCTION(array_create_rand)
{
	long num_items, items_type = COMUTO_ARRAY_RAND_TYPE_STRING, index, generated_long;
	ulong i;
	char *generated_string = NULL;
	HashTable *table;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &num_items, &items_type) == FAILURE) {
		return;
	}

	if(num_items <= 0) {
		php_error_docref(NULL, E_WARNING, "Size should be a positive integer");
		return;
	}

	array_init_size(return_value, num_items);

	for(i=0; i<num_items; i++) {
		switch (items_type) {
			case (COMUTO_ARRAY_RAND_TYPE_STRING | COMUTO_ARRAY_RAND_TYPE_INT):
				GET_RANDOM_NUMBER(index, 0, 1);
				if(index) {
					generate_random_string(&generated_string);
					add_next_index_string(return_value, generated_string, 0);
				} else {
					generate_random_long(generated_long);
					add_next_index_long(return_value, generated_long);
				}
			break;
			case COMUTO_ARRAY_RAND_TYPE_STRING:
				generate_random_string(&generated_string);
				add_next_index_string(return_value, generated_string, 0);
			break;
			case COMUTO_ARRAY_RAND_TYPE_INT:
				generate_random_long(generated_long);
				add_next_index_long(return_value, generated_long);
			break;
			default:
				php_error_docref(NULL, E_WARNING, "Unknown type");
			return;
		}
	}
}

COM_FUNCTION(array_stats)
{
	HashTable *array = NULL, *mem_usage_cache = NULL;
	zval *llstats = NULL, *input;
	int i, *j = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &input) == FAILURE) {
		return;
	}
	array = Z_ARRVAL_P(input);
	array_init_size(return_value, 8);
	ALLOC_HASHTABLE(mem_usage_cache);
	zend_hash_init(mem_usage_cache, array->nTableSize, NULL, NULL, 0);
	ALLOC_INIT_ZVAL(llstats);
	array_init_size(llstats, array->nTableSize);

	add_assoc_long(return_value, "array_size", array->nTableSize);
	add_assoc_long(return_value, "array_struct_memory_consumption", COMMON_ZVAL_SIZE + COMMON_HT_SIZE(array));
	add_assoc_long(return_value, "array_memory_consumption", get_var_memory_usage_ex(input, mem_usage_cache));
	add_assoc_long(return_value, "refcount", Z_REFCOUNT_P(input)-1);
	add_assoc_bool(return_value, "persistent_alloc", array->persistent);

	j = (int *)ecalloc(array->nTableSize, sizeof(int));
	Bucket *p = NULL;
	for(i=0; i<array->nTableSize; i++) {
		if(!array->arBuckets[i]) {
			continue;
		}
		j[i]++;
		if(!(p = array->arBuckets[i]->pNext)) {
			continue;
		}
		do {
			j[i]++;
		}while(p = p->pNext);
	}

	for(i=0; i<array->nTableSize; i++) {
		add_index_long(llstats, i, j[i]);
	}
	add_assoc_zval(return_value, "internal_ll_stats", llstats);
	efree(j);
	CLEAR_HASHTABLE(mem_usage_cache);
}

COM_FUNCTION(get_var_memory_usage)
{
	zval *arg = NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
		return;
	}
	HashTable *cache;
	ALLOC_HASHTABLE(cache);
	zend_hash_init(cache, 8, NULL, NULL, 0);

	size_t size = get_var_memory_usage_ex(arg, cache);
	CLEAR_HASHTABLE(cache);
	RETURN_LONG(size);
}


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

static size_t get_var_memory_usage_ex(zval *val, HashTable *zval_cache)
{
	size_t *size;
	if(zend_hash_index_exists(zval_cache, (ulong)val)) {
		return sizeof(zval *);
	} else {
		zend_hash_index_update(zval_cache, (ulong)val, &size, sizeof(void *), NULL);
		size_t zvalsize = COMMON_ZVAL_SIZE;
		switch(Z_TYPE_P(val)) {
			case IS_LONG:
			case IS_BOOL:
			case IS_DOUBLE:
			case IS_NULL:
			break;
			case IS_STRING:
				zvalsize += (size_t)Z_STRLEN_P(val);
			break;
			case IS_RESOURCE:
				zvalsize += (sizeof(zend_rsrc_list_dtors_entry) + sizeof(zend_rsrc_list_entry));
			break;
			case IS_OBJECT:
				zvalsize += (sizeof(zend_class_entry) + sizeof(zend_object));
				/*
				 * We should here be more accurate by computing the CE weight as well, this
				 * however is more difficult as several objects could share the same CE. We leave it for later :)
				 * + sizeof(zval*) * Z_OBJCE_P(val)->default_properties_count + sizeof(zval*) * Z_OBJCE_P(val)->default_static_members_count)
				 */
				zend_object *obj = (zend_object *)zend_object_store_get_object(val);
				if ((obj)->properties) {
					zvalsize += COMMON_HT_SIZE(obj->properties);
					ADD_SIZE_FROM_HT(obj->properties);
				}
				if(obj->guards) {
					zvalsize += COMMON_HT_SIZE(obj->guards);
					ADD_SIZE_FROM_HT(obj->guards);
				}
			break;
			case IS_ARRAY:
				zvalsize += COMMON_HT_SIZE(Z_ARRVAL_P(val));
				ADD_SIZE_FROM_HT(Z_ARRVAL_P(val));
			break;
			default:
				zend_error(E_WARNING, "Unknown variable type");
				return (size_t)0UL;
			break;
		}
		return zvalsize;
	}
}
