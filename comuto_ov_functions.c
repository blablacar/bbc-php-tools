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

#include "comuto_ov_functions.h"
#include "php_comuto.h"
#include "php.h"
#include "ext/standard/php_standard.h"

COM_FUNCTION(ini_get)
{
	char *varname, *str;
	int varname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
		return;
	}

	str = zend_ini_string(varname, varname_len + 1, 0);

	if (!str) {
		zend_error(E_NOTICE, INI_VALUE_INEXISTANT, varname);
		RETURN_FALSE;
	}

	RETURN_STRING(str, 1);
}

static int php_ini_check_path(char *option_name, int option_len, char *new_option_name, int new_option_len) /* {{{ */
{
	if (option_len != (new_option_len - 1)) {
		return 0;
	}

	return !strncmp(option_name, new_option_name, option_len);
}

COM_FUNCTION(ini_set)
{
	char *varname, *new_value;
	int varname_len, new_value_len;
	char *old_value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &varname, &varname_len, &new_value, &new_value_len) == FAILURE) {
		return;
	}

	old_value = zend_ini_string(varname, varname_len + 1, 0);

	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value, 1);
	} else {
		zend_error(E_NOTICE, INI_VALUE_INEXISTANT, varname);
		RETVAL_FALSE;
	}

	if (PG(open_basedir)) {
#define _CHECK_PATH(var, var_len, ini) php_ini_check_path(var, var_len, ini, sizeof(ini))
		if (_CHECK_PATH(varname, varname_len, "error_log") ||
			_CHECK_PATH(varname, varname_len, "java.class.path") ||
			_CHECK_PATH(varname, varname_len, "java.home") ||
			_CHECK_PATH(varname, varname_len, "mail.log") ||
			_CHECK_PATH(varname, varname_len, "java.library.path") ||
			_CHECK_PATH(varname, varname_len, "vpopmail.directory")) {
			if (php_check_open_basedir(new_value TSRMLS_CC)) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}
		}
	}

	if (zend_alter_ini_entry_ex(varname, varname_len + 1, new_value, new_value_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0 TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}

static int com_restore_ini_entry_ex(zend_ini_entry *ini_entry, int stage TSRMLS_DC) /* {{{ */
{
	int result = FAILURE;

	if (ini_entry->on_modify) {
		zend_try {
		/* even if on_modify bails out, we have to continue on with restoring,
			since there can be allocated variables that would be freed on MM shutdown
			and would lead to memory corruption later ini entry is modified again */
			result = ini_entry->on_modify(ini_entry, ini_entry->orig_value, ini_entry->orig_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage TSRMLS_CC);
		} zend_end_try();
	}
	if (stage == ZEND_INI_STAGE_RUNTIME && result == FAILURE) {
		/* runtime failure is OK */
		return FAILURE;
	}
	if (ini_entry->value != ini_entry->orig_value) {
		efree(ini_entry->value);
	}
	ini_entry->value = ini_entry->orig_value;
	ini_entry->value_length = ini_entry->orig_value_length;
	ini_entry->modifiable = ini_entry->orig_modifiable;
	ini_entry->modified = 0;
	ini_entry->orig_value = NULL;
	ini_entry->orig_value_length = 0;
	ini_entry->orig_modifiable = 0;
	return SUCCESS;
}

static int com_restore_ini_entry(char *name, uint name_length) /* {{{ */
{
	zend_ini_entry *ini_entry;
	TSRMLS_FETCH();

	if (zend_hash_find(EG(ini_directives), name, name_length, (void **) &ini_entry) == FAILURE) {
		zend_error(E_NOTICE, INI_VALUE_INEXISTANT, name);
		return FAILURE;
	} else if (!(ini_entry->modifiable & ZEND_INI_USER)) {
		zend_error(E_NOTICE, "You are not allowed to modify %s", name);
		return FAILURE;
	}

	if (!EG(modified_ini_directives) || !ini_entry->modified) {
		zend_error(E_NOTICE, "ini value %s has never been changed or it's already been restored, no need to restore", name);
		return FAILURE;
	}

	if (com_restore_ini_entry_ex(ini_entry, PHP_INI_STAGE_RUNTIME TSRMLS_CC) == SUCCESS) {
		zend_hash_del(EG(modified_ini_directives), name, name_length);
	} else {
		zend_error(E_WARNING, "Could not restore ini value %s", name);
		return FAILURE;
	}
	return SUCCESS;
}

COM_FUNCTION(ini_restore)
{
	char *varname;
	int varname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
		return;
	}

	com_restore_ini_entry(varname, varname_len+1);
}

static int com_add_config_entry_cb(zval *entry TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *retval = (zval *)va_arg(args, zval*);
	zval *tmp;

	if (Z_TYPE_P(entry) == IS_STRING) {
		if (hash_key->nKeyLength > 0) {
			add_assoc_stringl_ex(retval, hash_key->arKey, hash_key->nKeyLength, Z_STRVAL_P(entry), Z_STRLEN_P(entry), 1);
		} else {
			add_index_stringl(retval, hash_key->h, Z_STRVAL_P(entry), Z_STRLEN_P(entry), 1);
		}
	} else if (Z_TYPE_P(entry) == IS_ARRAY) {
		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		zend_hash_apply_with_arguments(Z_ARRVAL_P(entry) TSRMLS_CC, (apply_func_args_t) com_add_config_entry_cb, 1, tmp);
		add_assoc_zval_ex(retval, hash_key->arKey, hash_key->nKeyLength, tmp);
	}
	return 0;
}

COM_FUNCTION(get_cfg_var)
{
	char *varname;
	int varname_len;
	zval *retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
		return;
	}

	retval = cfg_get_entry(varname, varname_len + 1);

	if (retval) {
		if (Z_TYPE_P(retval) == IS_ARRAY) {
			array_init(return_value);
			zend_hash_apply_with_arguments(Z_ARRVAL_P(retval) TSRMLS_CC, (apply_func_args_t) com_add_config_entry_cb, 1, return_value);
			return;
		} else {
			RETURN_STRING(Z_STRVAL_P(retval), 1);
		}
	} else {
		zend_error(E_NOTICE, "Configuration value %s doesn't exist", varname);
		RETURN_FALSE;
	}
}
