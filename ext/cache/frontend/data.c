
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

#include "cache/frontend/data.h"
#include "cache/frontendinterface.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/fcall.h"
#include "kernel/variables.h"

/**
 * Dao\Cache\Frontend\Data
 *
 * Allows to cache native PHP data in a serialized form
 *
 *<code>
 *
 *	// Cache the files for 2 days using a Data frontend
 *	$frontCache = new Dao\Cache\Frontend\Data(array(
 *		"lifetime" => 172800
 *	));
 *
 *	// Create the component that will cache "Data" to a "File" backend
 *	// Set the cache file directory - important to keep the "/" at the end of
 *	// of the value for the folder
 *	$cache = new Dao\Cache\Backend\File($frontCache, array(
 *		"cacheDir" => "../app/cache/"
 *	));
 *
 *	// Try to get cached records
 *	$cacheKey = 'robots_order_id.cache';
 *	$robots    = $cache->get($cacheKey);
 *	if ($robots === null) {
 *
 *		// $robots is null due to cache expiration or data does not exist
 *		// Make the database call and populate the variable
 *		$robots = Robots::find(array("order" => "id"));
 *
 *		// Store it in the cache
 *		$cache->save($cacheKey, $robots);
 *	}
 *
 *	// Use $robots :)
 *	foreach ($robots as $robot) {
 *		echo $robot->name, "\n";
 *	}
 *</code>
 */
zend_class_entry *dao_cache_frontend_data_ce;

PHP_METHOD(Dao_Cache_Frontend_Data, __construct);
PHP_METHOD(Dao_Cache_Frontend_Data, getLifetime);
PHP_METHOD(Dao_Cache_Frontend_Data, isBuffering);
PHP_METHOD(Dao_Cache_Frontend_Data, start);
PHP_METHOD(Dao_Cache_Frontend_Data, getContent);
PHP_METHOD(Dao_Cache_Frontend_Data, stop);
PHP_METHOD(Dao_Cache_Frontend_Data, beforeStore);
PHP_METHOD(Dao_Cache_Frontend_Data, afterRetrieve);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_cache_frontend_data___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, frontendOptions)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_cache_frontend_data_method_entry[] = {
	PHP_ME(Dao_Cache_Frontend_Data, __construct, arginfo_dao_cache_frontend_data___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Cache_Frontend_Data, getLifetime, arginfo_dao_cache_frontendinterface_getlifetime, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Frontend_Data, isBuffering, arginfo_dao_cache_frontendinterface_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Frontend_Data, start, arginfo_dao_cache_frontendinterface_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Frontend_Data, getContent, arginfo_dao_cache_frontendinterface_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Frontend_Data, stop, arginfo_dao_cache_frontendinterface_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Frontend_Data, beforeStore, arginfo_dao_cache_frontendinterface_beforestore, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Frontend_Data, afterRetrieve, arginfo_dao_cache_frontendinterface_afterretrieve, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Cache\Frontend\Data initializer
 */
DAO_INIT_CLASS(Dao_Cache_Frontend_Data){

	DAO_REGISTER_CLASS(Dao\\Cache\\Frontend, Data, cache_frontend_data, dao_cache_frontend_data_method_entry, 0);

	zend_declare_property_null(dao_cache_frontend_data_ce, SL("_frontendOptions"), ZEND_ACC_PROTECTED);

	zend_class_implements(dao_cache_frontend_data_ce, 1, dao_cache_frontendinterface_ce);

	return SUCCESS;
}

/**
 * Dao\Cache\Frontend\Data constructor
 *
 * @param array $frontendOptions
 */
PHP_METHOD(Dao_Cache_Frontend_Data, __construct){

	zval *frontend_options = NULL;

	dao_fetch_params(0, 0, 1, &frontend_options);

	if (frontend_options) {
		dao_update_property(getThis(), SL("_frontendOptions"), frontend_options);
	}
}

/**
 * Returns cache lifetime
 *
 * @return int
 */
PHP_METHOD(Dao_Cache_Frontend_Data, getLifetime){

	zval options = {}, lifetime = {};

	dao_read_property(&options, getThis(), SL("_frontendOptions"), PH_NOISY|PH_READONLY);

	if (dao_array_isset_fetch_str(&lifetime, &options, SL("lifetime"), PH_READONLY) && Z_TYPE(lifetime) == IS_LONG) {
		RETURN_CTOR(&lifetime);
	} else {
		RETURN_LONG(1);
	}
}

/**
 * Check whether if frontend is buffering output
 *
 * @return boolean
 */
PHP_METHOD(Dao_Cache_Frontend_Data, isBuffering){


	RETURN_FALSE;
}

/**
 * Starts output frontend. Actually, does nothing
 */
PHP_METHOD(Dao_Cache_Frontend_Data, start){



}

/**
 * Returns output cached content
 *
 * @return string
 */
PHP_METHOD(Dao_Cache_Frontend_Data, getContent){


	RETURN_NULL();
}

/**
 * Stops output frontend
 */
PHP_METHOD(Dao_Cache_Frontend_Data, stop){



}

/**
 * Serializes data before storing them
 *
 * @param mixed $data
 * @return string
 */
PHP_METHOD(Dao_Cache_Frontend_Data, beforeStore){

	zval *data;

	dao_fetch_params(0, 1, 0, &data);
	dao_serialize(return_value, data);
}

/**
 * Unserializes data after retrieval
 *
 * @param mixed $data
 * @return mixed
 */
PHP_METHOD(Dao_Cache_Frontend_Data, afterRetrieve){

	zval *data;

	dao_fetch_params(0, 1, 0, &data);
	dao_unserialize(return_value, data);
}
