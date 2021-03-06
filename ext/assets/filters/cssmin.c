
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

#include "assets/filters/cssmin.h"
#ifdef DAO_NON_FREE
#include "assets/filters/cssminifier.h"
#else
#include "assets/filters/nocssminifier.h"
#endif
#include "assets/filterinterface.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"

/**
 * Dao\Assets\Filters\Cssmin
 *
 * Minify the css - removes comments
 * removes newlines and line feeds keeping
 * removes last semicolon from last property
 */
zend_class_entry *dao_assets_filters_cssmin_ce;

PHP_METHOD(Dao_Assets_Filters_Cssmin, filter);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_assets_filters_cssmin_filter, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_assets_filters_cssmin_method_entry[] = {
	PHP_ME(Dao_Assets_Filters_Cssmin, filter, arginfo_dao_assets_filters_cssmin_filter, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Assets\Filters\Cssmin initializer
 */
DAO_INIT_CLASS(Dao_Assets_Filters_Cssmin){

	DAO_REGISTER_CLASS(Dao\\Assets\\Filters, Cssmin, assets_filters_cssmin, dao_assets_filters_cssmin_method_entry, 0);

	zend_class_implements(dao_assets_filters_cssmin_ce, 1, dao_assets_filterinterface_ce);
	return SUCCESS;
}

/**
 * Filters the content using CSSMIN
 *
 * @param string $content
 * @return $content
 */
PHP_METHOD(Dao_Assets_Filters_Cssmin, filter){

	zval *content;

	dao_fetch_params(0, 1, 0, &content);
	
	if (dao_cssmin(return_value, content) == FAILURE) {
		return;
	}
}
