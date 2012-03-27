#ifndef COMUTO_OV_FUNCTIONS_H_
#define COMUTO_OV_FUNCTIONS_H_

#include "php_comuto.h"

#define INI_VALUE_INEXISTANT "ini value %s doesn't exist"

COM_FUNCTION(ini_get);
COM_FUNCTION(ini_set);
COM_FUNCTION(ini_restore);
COM_FUNCTION(get_cfg_var);

#endif /* COMUTO_OV_FUNCTIONS_H_ */
