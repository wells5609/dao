
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

#include "db.h"
#include "db/exception.h"

#include <ext/pdo/php_pdo_driver.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/exception.h"
#include "kernel/array.h"
#include "kernel/fcall.h"

/**
 * Dao\Db
 *
 * Dao\Db and its related classes provide a simple SQL database interface for Dao Framework.
 * The Dao\Db is the basic class you use to connect your PHP application to an RDBMS.
 * There is a different adapter class for each brand of RDBMS.
 *
 * This component is intended to lower level database operations. If you want to interact with databases using
 * higher level of abstraction use Dao\Mvc\Model.
 *
 * Dao\Db is an abstract class. You only can use it with a database adapter like Dao\Db\Adapter\Pdo
 *
 * <code>
 *
 *try {
 *
 *  $connection = new Dao\Db\Adapter\Pdo\Mysql(array(
 *     'host' => '192.168.0.11',
 *     'username' => 'sigma',
 *     'password' => 'secret',
 *     'dbname' => 'blog',
 *     'port' => '3306',
 *  ));
 *
 *  $result = $connection->query("SELECT * FROM robots LIMIT 5");
 *  $result->setFetchMode(Dao\Db::FETCH_NUM);
 *  while ($robot = $result->fetch()) {
 *    print_r($robot);
 *  }
 *
 *} catch (Dao\Db\Exception $e) {
 *	echo $e->getMessage(), PHP_EOL;
 *}
 *
 * </code>
 */
zend_class_entry *dao_db_ce;

PHP_METHOD(Dao_Db, setup);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_db_setup, 0, 0, 1)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_db_method_entry[] = {
	PHP_ME(Dao_Db, setup, arginfo_dao_db_setup, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

/**
 * Dao\Db initializer
 */
DAO_INIT_CLASS(Dao_Db){

	DAO_REGISTER_CLASS(Dao, Db, db, dao_db_method_entry, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_USE_DEFAULT"), (long int)PDO_FETCH_USE_DEFAULT);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_LAZY"),        (long int)PDO_FETCH_LAZY);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_ASSOC"),       (long int)PDO_FETCH_ASSOC);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_ARRAY"),       (long int)PDO_FETCH_ASSOC);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_NUM"),         (long int)PDO_FETCH_NUM);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_BOTH"),        (long int)PDO_FETCH_BOTH);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_OBJ"),         (long int)PDO_FETCH_OBJ);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_OBJECT"),      (long int)PDO_FETCH_OBJ);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_BOUND"),       (long int)PDO_FETCH_BOUND);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_COLUMN"),      (long int)PDO_FETCH_COLUMN);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_CLASS"),       (long int)PDO_FETCH_CLASS);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_INTO"),        (long int)PDO_FETCH_INTO);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_FUNC"),        (long int)PDO_FETCH_FUNC);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_NAMED"),       (long int)PDO_FETCH_NAMED);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_KEY_PAIR") ,   (long int)PDO_FETCH_KEY_PAIR);

	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_GROUP"),       (long int)PDO_FETCH_GROUP);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_UNIQUE"),      (long int)PDO_FETCH_UNIQUE);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_CLASSTYPE"),   (long int)PDO_FETCH_CLASSTYPE);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_SERIALIZE"),   (long int)PDO_FETCH_SERIALIZE);
	zend_declare_class_constant_long(dao_db_ce, SL("FETCH_PROPS_LATE"),  (long int)PDO_FETCH_PROPS_LATE);

	return SUCCESS;
}

/**
 * Enables/disables options in the Database component
 *
 * @param array $options
 */
PHP_METHOD(Dao_Db, setup){

	zval *options, escape_identifiers = {};

	dao_fetch_params(0, 1, 0, &options);

	if (Z_TYPE_P(options) != IS_ARRAY) {
		DAO_THROW_EXCEPTION_STR(dao_db_exception_ce, "Options must be an array");
		return;
	}

	/**
	 * Enables/Disables globally the escaping of SQL identifiers
	 */
	if (dao_array_isset_fetch_str(&escape_identifiers, options, SL("escapeSqlIdentifiers"), PH_READONLY)) {
		DAO_GLOBAL(db).escape_identifiers = zend_is_true(&escape_identifiers);
	}
}
