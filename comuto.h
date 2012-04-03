#ifndef COMUTO_H_
#define COMUTO_H_

#include "php_comuto.h"

#define COM_ARRAY_RAND_TYPE_STRING 1
#define COM_ARRAY_RAND_TYPE_INT    2
#define RANDOM_STRING_SIZE 42 /* yes */

#define GET_RANDOM_NUMBER(_number, _min, _max) {(_number) = php_rand(TSRMLS_C); RAND_RANGE((_number), (_min), (_max), PHP_RAND_MAX);}
#define generate_random_long(_long) (GET_RANDOM_NUMBER((_long), 0, INT_MAX))

#define COMMON_ZVAL_SIZE sizeof(zval_gc_info)

#define ADD_SIZE_FROM_HT(_ht) do { HashPosition pos; \
				zval **arraydata = NULL; \
				zend_hash_internal_pointer_reset_ex((_ht), &pos); \
				while(zend_hash_has_more_elements_ex((_ht), &pos) == SUCCESS) { \
					zend_hash_get_current_data_ex((_ht), (void **)&arraydata, &pos); \
					zvalsize += get_var_memory_usage_ex(*arraydata, zval_cache); \
					zend_hash_move_forward_ex((_ht), &pos); \
				} \
				} while(0)

#define COMMON_HT_SIZE(_ht) (sizeof(HashTable) + (_ht)->nTableSize * sizeof(Bucket *))

#endif /* COMUTO_H_ */
