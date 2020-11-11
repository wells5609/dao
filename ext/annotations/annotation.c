
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
  +------------------------------------------------------------------------+
*/

#include "annotations/annotation.h"
#include "annotations/exception.h"
#include "annotations/scanner.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/concat.h"


/**
 * Dao\Annotations\Annotation
 *
 * Represents a single annotation in an annotations collection
 */
zend_class_entry *dao_annotations_annotation_ce;

PHP_METHOD(Dao_Annotations_Annotation, __construct);
PHP_METHOD(Dao_Annotations_Annotation, getName);
PHP_METHOD(Dao_Annotations_Annotation, getExpression);
PHP_METHOD(Dao_Annotations_Annotation, getExprArguments);
PHP_METHOD(Dao_Annotations_Annotation, getArguments);
PHP_METHOD(Dao_Annotations_Annotation, numberArguments);
PHP_METHOD(Dao_Annotations_Annotation, getArgument);
PHP_METHOD(Dao_Annotations_Annotation, hasArgument);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_annotations_annotation___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, reflectionData, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_annotations_annotation_getexpression, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, expr, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_annotations_annotation_getargument, 0, 0, 1)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_annotations_annotation_hasargument, 0, 0, 1)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_annotations_annotation_method_entry[] = {
	PHP_ME(Dao_Annotations_Annotation, __construct, arginfo_dao_annotations_annotation___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Annotations_Annotation, getName, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Annotations_Annotation, getExpression, arginfo_dao_annotations_annotation_getexpression, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Annotations_Annotation, getExprArguments, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Annotations_Annotation, getArguments, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Annotations_Annotation, numberArguments, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Annotations_Annotation, getArgument, arginfo_dao_annotations_annotation_getargument, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Annotations_Annotation, hasArgument, arginfo_dao_annotations_annotation_hasargument, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Dao_Annotations_Annotation, getNamedArgument, getArgument, arginfo_dao_annotations_annotation_getargument, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Dao_Annotations_Annotation, getNamedParameter, getArgument, arginfo_dao_annotations_annotation_getargument, ZEND_ACC_PUBLIC | ZEND_ACC_DEPRECATED)
	PHP_MALIAS(Dao_Annotations_Annotation, hasNamedArgument, hasArgument, arginfo_dao_annotations_annotation_hasargument, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Annotations\Annotation initializer
 */
DAO_INIT_CLASS(Dao_Annotations_Annotation){

	DAO_REGISTER_CLASS(Dao\\Annotations, Annotation, annotations_annotation, dao_annotations_annotation_method_entry, 0);

	zend_declare_property_null(dao_annotations_annotation_ce, SL("_name"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_annotations_annotation_ce, SL("_arguments"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_annotations_annotation_ce, SL("_exprArguments"), ZEND_ACC_PROTECTED);

	return SUCCESS;
}

/**
 * Dao\Annotations\Annotation constructor
 *
 * @param array $reflectionData
 */
PHP_METHOD(Dao_Annotations_Annotation, __construct){

	zval *reflection_data, name = {}, arguments = {}, expr_arguments = {}, *argument;

	dao_fetch_params(0, 1, 0, &reflection_data);

	dao_array_fetch_str(&name, reflection_data, SL("name"), PH_NOISY|PH_READONLY);
	dao_update_property(getThis(), SL("_name"), &name);

	/**
	 * Process annotation arguments
	 */
	if (dao_array_isset_fetch_str(&expr_arguments, reflection_data, SL("arguments"), PH_READONLY)) {
		array_init(&arguments);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(expr_arguments), argument) {
			zval expr = {}, resolved_argument = {}, name = {};

			dao_array_fetch_str(&expr, argument, SL("expr"), PH_NOISY|PH_READONLY);

			DAO_CALL_METHOD(&resolved_argument, getThis(), "getexpression", &expr);
			if (dao_array_isset_fetch_str(&name, argument, SL("name"), PH_READONLY)) {
				dao_array_update(&arguments, &name, &resolved_argument, 0);
			} else {
				dao_array_append(&arguments, &resolved_argument, 0);
			}
		} ZEND_HASH_FOREACH_END();

		dao_update_property(getThis(), SL("_arguments"), &arguments);
		dao_update_property(getThis(), SL("_exprArguments"), &expr_arguments);
		zval_ptr_dtor(&arguments);
	}
}

/**
 * Returns the annotation's name
 *
 * @return string
 */
PHP_METHOD(Dao_Annotations_Annotation, getName){


	RETURN_MEMBER(getThis(), "_name");
}

/**
 * Resolves an annotation expression
 *
 * @param array $expr
 * @return mixed
 */
PHP_METHOD(Dao_Annotations_Annotation, getExpression){

	zval *expr, type = {}, items = {}, *item, exception_message = {};

	dao_fetch_params(0, 1, 0, &expr);

	dao_array_fetch_str(&type, expr, SL("type"), PH_NOISY|PH_READONLY);

	switch (dao_get_intval(&type)) {
		case PHANNOT_T_INTEGER:
		case PHANNOT_T_DOUBLE:
		case PHANNOT_T_STRING:
		case PHANNOT_T_IDENTIFIER:
			dao_array_fetch_str(&items, expr, SL("value"), PH_NOISY|PH_READONLY);
			RETURN_CTOR(&items);
			/* no break because of implicit return */

		case PHANNOT_T_NULL:
			RETURN_NULL();
			/* no break because of implicit return */

		case PHANNOT_T_FALSE:
			RETURN_FALSE;
			/* no break because of implicit return */

		case PHANNOT_T_TRUE:
			RETURN_TRUE;
			/* no break because of implicit return */

		case PHANNOT_T_ARRAY:
			dao_array_fetch_str(&items, expr, SL("items"), PH_NOISY|PH_READONLY);

			array_init(return_value);

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL(items), item) {
				zval name = {}, item_expr = {}, resolved_item = {};

				dao_array_fetch_str(&item_expr, item, SL("expr"), PH_NOISY|PH_READONLY);

				DAO_CALL_METHOD(&resolved_item, getThis(), "getexpression", &item_expr);
				if (dao_array_isset_fetch_str(&name, item, SL("name"), PH_READONLY)) {
					dao_array_update(return_value, &name, &resolved_item, 0);
				} else {
					dao_array_append(return_value, &resolved_item, 0);
				}
			} ZEND_HASH_FOREACH_END();

			return;
			/* no break because of implicit return */

		case PHANNOT_T_ANNOTATION:
			object_init_ex(return_value, dao_annotations_annotation_ce);
			DAO_CALL_METHOD(NULL, return_value, "__construct", expr);
			return;
			/* no break because of implicit return */

		default:
			DAO_CONCAT_SVS(&exception_message, "The expression ", &type, " is unknown");
			DAO_THROW_EXCEPTION_ZVAL(dao_annotations_exception_ce, &exception_message);
			return;
	}
}

/**
 * Returns the expression arguments without resolving
 *
 * @return array
 */
PHP_METHOD(Dao_Annotations_Annotation, getExprArguments){


	RETURN_MEMBER(getThis(), "_exprArguments");
}

/**
 * Returns the expression arguments
 *
 * @return array
 */
PHP_METHOD(Dao_Annotations_Annotation, getArguments){


	RETURN_MEMBER(getThis(), "_arguments");
}

/**
 * Returns the number of arguments that the annotation has
 *
 * @return int
 */
PHP_METHOD(Dao_Annotations_Annotation, numberArguments){

	zval arguments = {};

	dao_read_property(&arguments, getThis(), SL("_arguments"), PH_NOISY|PH_READONLY);
	dao_fast_count(return_value, &arguments);
}

/**
 * Returns an argument in a specific position
 *
 * @return mixed
 */
PHP_METHOD(Dao_Annotations_Annotation, getArgument){

	zval *position, arguments = {};

	dao_fetch_params(0, 1, 0, &position);

	dao_read_property(&arguments, getThis(), SL("_arguments"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(return_value, &arguments, position, PH_COPY)) {
		RETURN_NULL();
	}
}

/**
 * Checks if the annotation has a specific argument
 *
 * @return boolean
 */
PHP_METHOD(Dao_Annotations_Annotation, hasArgument){

	zval *position, arguments = {};

	dao_fetch_params(0, 1, 0, &position);

	dao_read_property(&arguments, getThis(), SL("_arguments"), PH_NOISY|PH_READONLY);
	RETURN_BOOL(dao_array_isset(&arguments, position));
}

/**
 * Returns a named argument
 *
 * @param string $name
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Annotations_Annotation, getNamedArgument);

/**
 * Returns a named argument (deprecated)
 *
 * @deprecated
 * @param string $name
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Annotations_Annotation, getNamedParameter);

/**
 * Checks if the annotation has a specific named argument
 *
 * @return boolean
 */
DAO_DOC_METHOD(Dao_Annotations_Annotation, hasNamedArgument);
