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

#include "db/builder/delete.h"
#include "db/builder/where.h"
#include "db/builder/exception.h"
#include "db/adapterinterface.h"
#include "di/injectable.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/string.h"

/**
 * Dao\Db\Builder\Delete
 */
zend_class_entry *dao_db_builder_delete_ce;

PHP_METHOD(Dao_Db_Builder_Delete, __construct);
PHP_METHOD(Dao_Db_Builder_Delete, _execute);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_db_builder_delete___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, db, IS_STRING, 1)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_db_builder_delete_method_entry[] = {
	PHP_ME(Dao_Db_Builder_Delete, __construct, arginfo_dao_db_builder_delete___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Db_Builder_Delete, _execute, arginfo_empty, ZEND_ACC_PROTECTED)
	PHP_FE_END
};

/**
 * Dao\Db\Builder\Delete initializer
 */
DAO_INIT_CLASS(Dao_Db_Builder_Delete){

	DAO_REGISTER_CLASS_EX(Dao\\Db\\Builder, Delete, db_builder_delete, dao_db_builder_where_ce, dao_db_builder_delete_method_entry, 0);

	return SUCCESS;
}

/**
 * Dao\Db\Builder\Delete constructor
 *
 * @param array $params
 * @param Dao\Di $dependencyInjector
 */
PHP_METHOD(Dao_Db_Builder_Delete, __construct){

	zval *tables, *db = NULL;

	dao_fetch_params(0, 1, 1, &tables, &db);

	if (Z_TYPE_P(tables) != IS_STRING && Z_TYPE_P(tables) != IS_ARRAY) {
		DAO_THROW_EXCEPTION_STR(dao_db_builder_exception_ce, "The tables must be string or array");
		return;
	}

	dao_update_property_array_str(getThis(), SL("_definition"), SL("tables"), tables);

	if (db && DAO_IS_NOT_EMPTY(db)) {
		dao_update_property(getThis(), SL("_defaultConnectionService"), db);
	}
}

/**
 * Returns a PHQL statement built based on the builder parameters
 *
 * @return string
 */
PHP_METHOD(Dao_Db_Builder_Delete, _execute){

	zval *pretreatment = NULL, definition = {}, conditions = {}, bind_params = {}, bind_types = {}, service = {}, dependency_injector = {}, connection = {}, dialect = {}, sql_delete = {};

	dao_fetch_params(1, 0, 1, &pretreatment);

	if (!pretreatment) {
		pretreatment = &DAO_GLOBAL(z_false);
	}

	dao_read_property(&definition, getThis(), SL("_definition"), PH_CTOR);
	DAO_MM_ADD_ENTRY(&definition);

	dao_read_property(&conditions, getThis(), SL("_conditions"), PH_READONLY);
	if (DAO_IS_NOT_EMPTY(&conditions)) {
		dao_array_update_str(&definition, SL("where"), &conditions, PH_COPY);
	}

	DAO_MM_CALL_SELF(&bind_params, "getbindparams");
	DAO_MM_ADD_ENTRY(&bind_params);

	DAO_MM_CALL_SELF(&bind_types, "getbindtypes");
	DAO_MM_ADD_ENTRY(&bind_types);

	dao_read_property(&service, getThis(), SL("_defaultConnectionService"), PH_READONLY);
	
	if (Z_TYPE(service) != IS_OBJECT) {
		if (DAO_IS_EMPTY(&service)) {
			DAO_MM_THROW_EXCEPTION_STR(dao_db_builder_exception_ce, "Invalid injected connection service");
			return;
		}

		DAO_MM_CALL_METHOD(&dependency_injector, getThis(), "getdi");
		DAO_MM_ADD_ENTRY(&dependency_injector);
		if (Z_TYPE(dependency_injector) != IS_OBJECT) {
			DAO_MM_THROW_EXCEPTION_STR(dao_db_builder_exception_ce, "A dependency injector container is required to obtain the services related to the ORM");
			return;
		}

		/**
		 * Request the connection service from the DI
		 */
		DAO_MM_CALL_METHOD(&connection, &dependency_injector, "getshared", &service);
		DAO_MM_ADD_ENTRY(&connection);
		if (Z_TYPE(connection) != IS_OBJECT) {
			DAO_THROW_EXCEPTION_STR(dao_db_builder_exception_ce, "Invalid injected connection service");
			return;
		}
	} else {
		ZVAL_COPY_VALUE(&connection, &service);
		DAO_MM_VERIFY_INTERFACE(&connection, dao_db_adapterinterface_ce);
	}

	DAO_MM_CALL_METHOD(&dialect, &connection, "getdialect");
	DAO_MM_ADD_ENTRY(&dialect);

	DAO_MM_CALL_METHOD(&sql_delete, &dialect, "delete", &definition);
	DAO_MM_ADD_ENTRY(&sql_delete);

	if (zend_is_true(pretreatment)) {
		array_init(return_value);
		dao_array_update_str(return_value, SL("sql"), &sql_delete, PH_COPY);
		dao_array_update_str(return_value, SL("variables"), &bind_params, PH_COPY);
		dao_array_update_str(return_value, SL("types"), &bind_types, PH_COPY);
	} else {
		/**
		 * Execute the query
		 */
		DAO_MM_CALL_METHOD(return_value, &connection, "execute", &sql_delete, &bind_params, &bind_types);
	}

	RETURN_MM();
}
