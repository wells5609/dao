
/*
  +------------------------------------------------------------------------+
  | Dao Framework                                                          |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#include "filter.h"
#include "filterinterface.h"
#include "filter/exception.h"
#include "filter/../date.h"

#include <Zend/zend_closures.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/hash.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "kernel/filter.h"
#include "kernel/concat.h"

/**
 * Dao\Filter
 *
 * The Dao\Filter component provides a set of commonly needed data filters. It provides
 * object oriented wrappers to the php filter extension. Also allows the developer to
 * define his/her own filters
 *
 *<code>
 *	$filter = new Dao\Filter();
 *	$filter->sanitize("some(one)@exa\\mple.com", "email"); // returns "someone@example.com"
 *	$filter->sanitize("hello<<", "string"); // returns "hello"
 *	$filter->sanitize("!100a019", "int"); // returns "100019"
 *	$filter->sanitize("!100a019.01a", "float"); // returns "100019.01"
 *</code>
 */
zend_class_entry *dao_filter_ce;

PHP_METHOD(Dao_Filter, __construct);
PHP_METHOD(Dao_Filter, add);
PHP_METHOD(Dao_Filter, sanitize);
PHP_METHOD(Dao_Filter, _sanitize);
PHP_METHOD(Dao_Filter, getFilters);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_filterinterface___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 1)
	ZEND_ARG_INFO(0, handler)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_filter_method_entry[] = {
	PHP_ME(Dao_Filter, __construct, arginfo_dao_filterinterface___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Filter, add, arginfo_dao_filterinterface_add, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Filter, sanitize, arginfo_dao_filterinterface_sanitize, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Filter, _sanitize, arginfo_empty, ZEND_ACC_PROTECTED)
	PHP_ME(Dao_Filter, getFilters, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Filter initializer
 */
DAO_INIT_CLASS(Dao_Filter){

	DAO_REGISTER_CLASS(Dao, Filter, filter, dao_filter_method_entry, 0);

	zend_declare_property_null(dao_filter_ce, SL("_filters"), ZEND_ACC_PROTECTED);
	zend_declare_property_string(dao_filter_ce, SL("_dateFormat"), "Y-m-d H:i:s", ZEND_ACC_PROTECTED);

	zend_declare_property_string(dao_filter_ce, SL("FILTER_EMAIL"), "email", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_ABSINT"), "absint", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_INT"), "int", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_INT_CAST"), "int!", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_STRING"), "string", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_FLOAT"), "float", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_FLOAT_CAST"), "float!", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_ALPHANUM"), "alphanum", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_TRIM"), "trim", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_STRIPTAGS"), "striptags", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_LOWER"), "lower", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_UPPER"), "upper", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_XSS"), "xss", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
	zend_declare_property_string(dao_filter_ce, SL("FILTER_ARRAY"), "array", ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);

	zend_class_implements(dao_filter_ce, 1, dao_filterinterface_ce);

	return SUCCESS;
}

/**
 * Dao\Filter constructor
 */
PHP_METHOD(Dao_Filter, __construct){

	zval *options = NULL, date_format = {};

	dao_fetch_params(0, 0, 1, options);

	if (!options) {
		options = &DAO_GLOBAL(z_null);
	}

	if (likely(Z_TYPE_P(options) != IS_ARRAY) 
		&& dao_array_isset_fetch_str(&date_format, options, SL("dateFormat"), PH_READONLY) 
		&& Z_TYPE(date_format) == IS_STRING) {

		dao_update_property(getThis(), SL("_dateFormat"), &date_format);
	}
}

/**
 * Adds a user-defined filter
 *
 * @param string $name
 * @param callable $handler
 * @return Dao\Filter
 */
PHP_METHOD(Dao_Filter, add){

	zval *name, *handler;

	dao_fetch_params(0, 2, 0, &name, &handler);

	if (Z_TYPE_P(handler) != IS_OBJECT && !dao_is_callable(handler)) {
		DAO_THROW_EXCEPTION_STR(dao_filter_exception_ce, "Filter must be an object or callable");
		return;
	}

	dao_update_property_array(getThis(), SL("_filters"), name, handler);
	RETURN_THIS();
}

/**
 * Sanitizes a value with a specified single or set of filters
 *
 * @param mixed $value
 * @param mixed $filters
 * @param boolean $recursive
 * @param array $options
 * @param int $recursiveLevel
 * @return mixed
 */
PHP_METHOD(Dao_Filter, sanitize){

	zval *value, *filters, *recursive = NULL, *options = NULL, *_recursive_level = NULL, recursive_level = {};
	zval new_value = {}, *item_value, *filter, filter_value = {}, sanizited_value = {};
	zend_string *filter_key, *item_key;
	ulong item_idx;

	dao_fetch_params(0, 2, 3, &value, &filters, &recursive, &options, &_recursive_level);

	if (!recursive || Z_TYPE_P(recursive) == IS_NULL) {
		recursive = &DAO_GLOBAL(z_true);
	}

	if (!options) {
		options = &DAO_GLOBAL(z_null);
	}

	if (!_recursive_level || Z_TYPE_P(_recursive_level) != IS_LONG) {
		ZVAL_LONG(&recursive_level, 0);
	} else {
		ZVAL_COPY(&recursive_level, _recursive_level);
	}

	/**
	 * Apply an array of filters
	 */
	if (Z_TYPE_P(filters) == IS_ARRAY) {
		ZVAL_DUP(&new_value, value);
		if (Z_TYPE_P(value) != IS_NULL) {
			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(filters), filter_key, filter) {
				zval real_filter = {}, real_options = {}, array_value = {};

				if (filter_key) {
					ZVAL_STR(&real_filter, filter_key);
					if (Z_TYPE_P(filter) == IS_ARRAY) {
						if (Z_TYPE_P(options) == IS_ARRAY) {
							dao_fast_array_merge(&real_options, options, filter);
						} else {
							ZVAL_COPY(&real_options, filter);
						}
					} else {
						if (Z_TYPE_P(options) == IS_ARRAY) {
							ZVAL_DUP(&real_options, options);
						} else {
							array_init(&real_options);
						}
						dao_array_update(&real_options, &real_filter, filter, PH_COPY);
					}
				} else {
					ZVAL_COPY(&real_filter, filter);
					ZVAL_COPY(&real_options, options);
				}

				/**
				 * If the value to filter is an array we apply the filters recursively
				 */
				if (Z_TYPE(new_value) == IS_ARRAY && zend_is_true(recursive)) {
					array_init(&array_value);

					dao_decrement(&recursive_level);
					ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(new_value), item_idx, item_key, item_value) {
						if (Z_TYPE_P(item_value) == IS_ARRAY && Z_LVAL(recursive_level) > 0) {
							DAO_CALL_METHOD(&filter_value, getThis(), "sanitize", item_value, filters, recursive, options, &recursive_level);
						} else {
							DAO_CALL_METHOD(&filter_value, getThis(), "_sanitize", item_value, &real_filter, &real_options);
						}
						if (item_key) {
							dao_array_update_string(&array_value, item_key, &filter_value, 0);
						} else {
							dao_array_update_long(&array_value, item_idx, &filter_value, 0);
						}
					} ZEND_HASH_FOREACH_END();
					zval_ptr_dtor(&new_value);
					ZVAL_COPY_VALUE(&new_value, &array_value);
				} else {
					DAO_CALL_METHOD(&filter_value, getThis(), "_sanitize", &new_value, &real_filter, &real_options);
					zval_ptr_dtor(&new_value);
					ZVAL_COPY_VALUE(&new_value, &filter_value);
				}
				zval_ptr_dtor(&real_filter);
				zval_ptr_dtor(&real_options);
			} ZEND_HASH_FOREACH_END();
		}
		RETURN_ZVAL(&new_value, 0, 0);
	}

	/**
	 * Apply a single filter value
	 */
	if (Z_TYPE_P(value) == IS_ARRAY && zend_is_true(recursive)) {
		array_init(&sanizited_value);
		dao_decrement(&recursive_level);
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(value), item_idx, item_key, item_value) {
			if (Z_TYPE_P(item_value) == IS_ARRAY && Z_LVAL(recursive_level) > 0) {
				DAO_CALL_METHOD(&filter_value, getThis(), "sanitize", item_value, filters, recursive, options, &recursive_level);
			} else {
				DAO_CALL_METHOD(&filter_value, getThis(), "_sanitize", item_value, filters, options);
			}
			if (item_key) {
				dao_array_update_string(&sanizited_value, item_key, &filter_value, 0);
			} else {
				dao_array_update_long(&sanizited_value, item_idx, &filter_value, 0);
			}
		} ZEND_HASH_FOREACH_END();

	} else {
		DAO_CALL_METHOD(&sanizited_value, getThis(), "_sanitize", value, filters, options);
	}

	RETURN_ZVAL(&sanizited_value, 0, 0);
}

/**
 * Internal sanitize wrapper to filter_var
 *
 * @param  mixed $value
 * @param  string $filter
 * @return mixed
 */
PHP_METHOD(Dao_Filter, _sanitize){

	zval *value, *filter, *options = NULL, filters = {}, filter_object = {}, arguments = {}, type = {}, quote = {}, empty_str = {};
	zval escaped = {}, filtered = {}, allow_fraction = {}, format = {}, exception_message = {};

	dao_fetch_params(0, 2, 1, &value, &filter, &options);

	if (!options) {
		options = &DAO_GLOBAL(z_null);
	}

	if (Z_TYPE_P(filter) != IS_STRING) {
		if (Z_TYPE_P(filter) == IS_OBJECT) {
			if (instanceof_function(Z_OBJCE_P(filter), zend_ce_closure)) {
				DAO_CALL_METHOD(return_value, filter, "call", getThis(), value);
				return;
			}

			DAO_RETURN_CALL_METHOD(filter, "filter", value);
			return;
		}
		if (dao_is_callable(filter)) {
			array_init_size(&arguments, 1);
			dao_array_append(&arguments, value, PH_COPY);
			DAO_CALL_USER_FUNC_ARRAY(return_value, filter, &arguments);
			zval_ptr_dtor(&arguments);
			return;
		}
		DAO_THROW_EXCEPTION_STR(dao_filter_exception_ce, "Filter must be an object or callable");
		return;
	}

	dao_read_property(&filters, getThis(), SL("_filters"), PH_NOISY|PH_READONLY);
	if (dao_array_isset_fetch(&filter_object, &filters, filter, PH_READONLY) && (Z_TYPE(filter_object) == IS_OBJECT || dao_is_callable(&filter_object))) {
		/**
		 * If the filter is a closure we call it in the PHP userland
		 */
		if (Z_TYPE(filter_object) == IS_OBJECT && instanceof_function(Z_OBJCE(filter_object), zend_ce_closure)) {
			DAO_CALL_METHOD(return_value, &filter_object, "call", getThis(), value);
			return;
		} else if (dao_is_callable(&filter_object)) {
			array_init_size(&arguments, 1);
			dao_array_append(&arguments, value, PH_COPY);
			DAO_CALL_USER_FUNC_ARRAY(return_value, &filter_object, &arguments);
			zval_ptr_dtor(&arguments);
			return;
		}

		DAO_RETURN_CALL_METHOD(&filter_object, "filter", value);
		return;
	}

	if (DAO_IS_STRING(filter, "email")) {
		/**
		 * The 'email' filter uses the filter extension
		 */
		ZVAL_LONG(&type, 517); /* FILTER_SANITIZE_EMAIL */
		ZVAL_STRING(&quote, "'");
		ZVAL_STRING(&empty_str, "");

		DAO_STR_REPLACE(&escaped, &quote, &empty_str, value);
		zval_ptr_dtor(&quote);
		zval_ptr_dtor(&empty_str);

		DAO_CALL_FUNCTION(&filtered, "filter_var", &escaped, &type);
		zval_ptr_dtor(&escaped);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "int")) {
		/**
		 * 'int' filter sanitizes a numeric input
		 */
		ZVAL_LONG(&type, 519); /* FILTER_SANITIZE_NUMBER_INT */

		DAO_CALL_FUNCTION(&filtered, "filter_var", value, &type);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "int!")) {
		ZVAL_DUP(&filtered, value);
		convert_to_long_base(&filtered, 10);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "int?")) {
		if (dao_is_numeric(value)) {
			ZVAL_ZVAL(&filtered, value, 1, 0);
			convert_to_long_base(&filtered, 10);
		} else {
			ZVAL_COPY_VALUE(&filtered, value);
		}
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "abs")) {
		convert_scalar_to_number_ex(value);
		if (Z_TYPE_P(value) == IS_DOUBLE) {
			ZVAL_DOUBLE(&filtered, fabs(Z_DVAL_P(value)));
		} else if (Z_DVAL_P(value) == IS_LONG) {
			if (Z_DVAL_P(value) == LONG_MIN) {
				ZVAL_DOUBLE(&filtered, -(double)LONG_MIN);
			} else {
				ZVAL_LONG(&filtered, Z_LVAL_P(value) < 0 ? -Z_LVAL_P(value) : Z_LVAL_P(value));
			}
		} else {
			ZVAL_FALSE(&filtered);
		}
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "string")) {
		ZVAL_LONG(&type, 513); /* FILTER_SANITIZE_STRING */

		DAO_CALL_FUNCTION(&filtered, "filter_var", value, &type);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "float")) {
		zval opt = {};
		/**
		 * The 'float' filter uses the filter extension
		 */
		ZVAL_LONG(&allow_fraction, 4096); /* FILTER_FLAG_ALLOW_FRACTION */

		array_init_size(&opt, 1);
		dao_array_update_str(&opt, SL("flags"), &allow_fraction, PH_COPY);

		ZVAL_LONG(&type, 520); /* FILTER_SANITIZE_NUMBER_FLOAT */

		DAO_CALL_FUNCTION(&filtered, "filter_var", value, &type, &opt);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "float!")) {
		ZVAL_ZVAL(&filtered, value, 1, 0);
		convert_to_double(&filtered);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "float?")) {
		if (dao_is_numeric(value)) {
			ZVAL_ZVAL(&filtered, value, 1, 0);
			convert_to_double(&filtered);
		} else {
			ZVAL_COPY_VALUE(&filtered, value);
		}
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "alphanum")) {
		dao_filter_alphanum(&filtered, value);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "trim")) {
		ZVAL_STR(&filtered, dao_trim(value, NULL, DAO_TRIM_BOTH));
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "striptags")) {
		dao_fast_strip_tags(&filtered, value);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "lower")) {
		if (dao_function_exists_ex(SL("mb_strtolower")) == SUCCESS) {
			/**
			 * 'lower' checks for the mbstring extension to make a correct lowercase
			 * transformation
			 */
			DAO_CALL_FUNCTION(&filtered, "mb_strtolower", value);
		} else {
			dao_fast_strtolower(&filtered, value);
		}
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "upper")) {
		if (dao_function_exists_ex(SL("mb_strtoupper")) == SUCCESS) {
			/**
			 * 'upper' checks for the mbstring extension to make a correct lowercase
			 * transformation
			 */
			DAO_CALL_FUNCTION(&filtered, "mb_strtoupper", value);
		} else {
			dao_fast_strtoupper(&filtered, value);
		}
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "daterange")) {
		zval format = {}, delimiter = {};
		if (Z_TYPE_P(options) == IS_ARRAY) {
			if (!dao_array_isset_fetch_str(&format, options, SL("format"), PH_READONLY)) {
				ZVAL_NULL(&format);
			}
			if (!dao_array_isset_fetch_str(&delimiter, options, SL("delimiter"), PH_COPY)) {
				ZVAL_STRING(&delimiter, " - ");
			}
		} else {
			ZVAL_NULL(&format);
			ZVAL_STRING(&delimiter, " - ");
		}
		DAO_CALL_CE_STATIC(&filtered, dao_date_ce, "filter", value, &format, &delimiter);
		zval_ptr_dtor(&delimiter);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "date")) {
		DAO_CALL_CE_STATIC(&filtered, dao_date_ce, "filter", value);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "datetime")) {
		if (Z_TYPE_P(options) != IS_ARRAY || !dao_array_isset_fetch_str(&format, options, SL("dateFormat"), PH_READONLY)) {
			dao_read_property(&format, getThis(), SL("_dateFormat"), PH_READONLY);
		}
		DAO_CALL_CE_STATIC(&filtered, dao_date_ce, "filter", value, &format);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "url")) {
		ZVAL_LONG(&type, 518); /* FILTER_SANITIZE_URL */

		DAO_CALL_FUNCTION(&filtered, "filter_var", value, &type);
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "ip")) {
		ZVAL_LONG(&type, 275); /* FILTER_VALIDATE_IP */

		DAO_CALL_FUNCTION(&filtered, "filter_var", value, &type);
		if (DAO_IS_FALSE(&filtered)) {
			ZVAL_NULL(&filtered);
		}
		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "in")) {
		if (Z_TYPE_P(options) != IS_ARRAY || !dao_fast_in_array(value, options)) {
			ZVAL_NULL(&filtered);
		} else {
			ZVAL_COPY(&filtered, value);
		}

		goto ph_end_0;
	}

	if (DAO_IS_STRING(filter, "array")) {
		convert_to_array(value);
		if (zend_is_true(options)) {
			if (dao_is_callable(options)) {
				DAO_CALL_FUNCTION(&filtered, "array_filter", value, options);
			} else {
				DAO_CALL_FUNCTION(&filtered, "array_filter", value);
			}
		} else {
			ZVAL_COPY(&filtered, value);
		}

		goto ph_end_0;
	}

	if (dao_is_callable(filter)) {
		array_init_size(&arguments, 1);
		dao_array_append(&arguments, value, PH_COPY);
		DAO_CALL_USER_FUNC_ARRAY(return_value, filter, &arguments);
		zval_ptr_dtor(&arguments);
		return;
	}
	
	DAO_CONCAT_SVS(&exception_message, "Sanitize filter ", filter, " is not supported");
	DAO_THROW_EXCEPTION_ZVAL(dao_filter_exception_ce, &exception_message);
	zval_ptr_dtor(&exception_message);
	return;

ph_end_0:

	RETURN_ZVAL(&filtered, 0, 0);
}

/**
 * Return the user-defined filters in the instance
 *
 * @return object[]
 */
PHP_METHOD(Dao_Filter, getFilters){

	RETURN_MEMBER(getThis(), "_filters");
}
