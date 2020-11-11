
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

#include "validation/validator/date.h"
#include "validation/validator.h"
#include "validation/validatorinterface.h"
#include "validation/message.h"
#include "validation/exception.h"
#include "validation.h"
#include "../../date.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/array.h"
#include "kernel/string.h"

#include "interned-strings.h"

/**
 * Dao\Validation\Validator\Date
 *
 * Checks if a value has a correct DATE format
 *
 *<code>
 *use Dao\Validation\Validator\Date as DateValidator;
 *
 *$validator->add('date', new DateValidator(array(
 *   'message' => 'The date is not valid'
 *)));
 *</code>
 */
zend_class_entry *dao_validation_validator_date_ce;

PHP_METHOD(Dao_Validation_Validator_Date, validate);
PHP_METHOD(Dao_Validation_Validator_Date, valid);

static const zend_function_entry dao_validation_validator_date_method_entry[] = {
	PHP_ME(Dao_Validation_Validator_Date, validate, arginfo_dao_validation_validatorinterface_validate, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Validation_Validator_Date, valid, arginfo_dao_validation_validatorinterface_valid, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Validation\Validator\Date initializer
 */
DAO_INIT_CLASS(Dao_Validation_Validator_Date){

	DAO_REGISTER_CLASS_EX(Dao\\Validation\\Validator, Date, validation_validator_date, dao_validation_validator_ce, dao_validation_validator_date_method_entry, 0);

	zend_class_implements(dao_validation_validator_date_ce, 1, dao_validation_validatorinterface_ce);

	return SUCCESS;
}

/**
 * Executes the validation
 *
 * @param Dao\Validation $validator
 * @param string $attribute
 * @return boolean
 */
PHP_METHOD(Dao_Validation_Validator_Date, validate){

	zval *validaton, *attribute, *_allow_empty = NULL, value = {}, allow_empty = {}, format = {}, valid = {}, label = {}, pairs = {}, message_str = {}, code = {}, prepared = {}, message = {};
	zend_class_entry *ce = Z_OBJCE_P(getThis());

	dao_fetch_params(0, 2, 1, &validaton, &attribute, &_allow_empty);
	DAO_VERIFY_INTERFACE_EX(validaton, dao_validationinterface_ce, dao_validation_exception_ce);

	DAO_CALL_METHOD(&value, validaton, "getvalue", attribute);

	RETURN_ON_FAILURE(dao_validation_validator_getoption_helper(&allow_empty, ce, getThis(), ISV(allowEmpty)));
	if (Z_TYPE(allow_empty) == IS_NULL) {
		if (_allow_empty && zend_is_true(_allow_empty)) {
			ZVAL_COPY(&allow_empty, _allow_empty);
		}
	}
	if (DAO_IS_EMPTY_STRING(&value)) {
		if (zend_is_true(&allow_empty)) {
			zval_ptr_dtor(&allow_empty);
			RETURN_TRUE;
		}
	}
	zval_ptr_dtor(&allow_empty);

	RETURN_ON_FAILURE(dao_validation_validator_getoption_helper(&format, ce, getThis(), "format"));

	DAO_CALL_SELF(&valid, "valid", &value, &format);
	zval_ptr_dtor(&value);
	zval_ptr_dtor(&format);

	if (DAO_IS_FALSE(&valid)) {
		RETURN_ON_FAILURE(dao_validation_validator_getoption_helper(&label, ce, getThis(), ISV(label)));
		if (!zend_is_true(&label)) {
			DAO_CALL_METHOD(&label, validaton, "getlabel", attribute);
		}

		array_init_size(&pairs, 1);
		dao_array_update_str(&pairs, SL(":field"), &label, PH_COPY);
		zval_ptr_dtor(&label);

		RETURN_ON_FAILURE(dao_validation_validator_getoption_helper(&message_str, ce, getThis(), ISV(message)));
		if (!zend_is_true(&message_str)) {
			RETURN_ON_FAILURE(dao_validation_getdefaultmessage_helper(&message_str, Z_OBJCE_P(validaton), validaton, "Date"));
		}

		RETURN_ON_FAILURE(dao_validation_validator_getoption_helper(&code, ce, getThis(), ISV(code)));
		if (Z_TYPE(code) == IS_NULL) {
			ZVAL_LONG(&code, 0);
		}

		DAO_CALL_FUNCTION(&prepared, "strtr", &message_str, &pairs);
		zval_ptr_dtor(&message_str);
		zval_ptr_dtor(&pairs);

		dao_validation_message_construct_helper(&message, &prepared, attribute, "Date", &code);
		zval_ptr_dtor(&prepared);

		DAO_CALL_METHOD(NULL, validaton, "appendmessage", &message);
		zval_ptr_dtor(&message);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

/**
 * Executes the validation
 *
 * @param string $value
 * @return boolean
 */
PHP_METHOD(Dao_Validation_Validator_Date, valid){

	zval *value, *format = NULL, valid = {};

	dao_fetch_params(0, 1, 1, &value, &format);

	if (!format) {
		format = &DAO_GLOBAL(z_null);
	}

	if (Z_TYPE_P(value) != IS_STRING || Z_STRLEN_P(value) <= 0) {
		RETURN_FALSE;
	}

	DAO_CALL_CE_STATIC(&valid, dao_date_ce, "valid", value, format);
	if (!zend_is_true(&valid)) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}