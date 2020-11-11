
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

#include "assets/resource/js.h"
#include "assets/resource.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"

/**
 * Dao\Assets\Resource\Js
 *
 * Represents Javascript resources
 */
zend_class_entry *dao_assets_resource_js_ce;

PHP_METHOD(Dao_Assets_Resource_Js, __construct);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_assets_resource_js___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, local)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_INFO(0, attributes)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_assets_resource_js_method_entry[] = {
	PHP_ME(Dao_Assets_Resource_Js, __construct, arginfo_dao_assets_resource_js___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_FE_END
};

/**
 * Dao\Assets\Resource\Js initializer
 */
DAO_INIT_CLASS(Dao_Assets_Resource_Js){

	DAO_REGISTER_CLASS_EX(Dao\\Assets\\Resource, Js, assets_resource_js, dao_assets_resource_ce, dao_assets_resource_js_method_entry, 0);

	return SUCCESS;
}

/**
 * Dao\Assets\Resource\Js
 *
 * @param string $path
 * @param boolean $local
 * @param boolean $filter
 * @param array $attributes
 */
PHP_METHOD(Dao_Assets_Resource_Js, __construct){

	zval *path, *local = NULL, *filter = NULL, *attributes = NULL, type = {};

	dao_fetch_params(0, 1, 3, &path, &local, &filter, &attributes);

	ZVAL_STRING(&type, "js");

	DAO_CALL_PARENT(NULL, dao_assets_resource_js_ce, getThis(), "__construct",	&type, path,
		(local ? local : &DAO_GLOBAL(z_true)),
		(filter ? filter : &DAO_GLOBAL(z_true)),
		(attributes ? attributes : &DAO_GLOBAL(z_null))
	);
	zval_ptr_dtor(&type);
}
