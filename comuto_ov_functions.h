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

#ifndef COMUTO_OV_FUNCTIONS_H_
#define COMUTO_OV_FUNCTIONS_H_

#include "php_comuto.h"

#define INI_VALUE_INEXISTANT "ini value %s doesn't exist"

COM_FUNCTION(ini_get);
COM_FUNCTION(ini_set);
COM_FUNCTION(ini_restore);
COM_FUNCTION(get_cfg_var);

#endif /* COMUTO_OV_FUNCTIONS_H_ */
