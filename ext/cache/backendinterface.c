
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

#include "cache/backendinterface.h"
#include "kernel/main.h"

zend_class_entry *dao_cache_backendinterface_ce;

static const zend_function_entry dao_cache_backendinterface_method_entry[] = {
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, start, arginfo_dao_cache_backendinterface_start)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, stop, arginfo_dao_cache_backendinterface_stop)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, getFrontend, arginfo_empty)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, getOptions, arginfo_empty)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, isFresh, arginfo_empty)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, isStarted, arginfo_empty)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, get, arginfo_dao_cache_backendinterface_get)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, save, arginfo_dao_cache_backendinterface_save)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, delete, arginfo_dao_cache_backendinterface_delete)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, queryKeys, arginfo_dao_cache_backendinterface_querykeys)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, exists, arginfo_dao_cache_backendinterface_exists)
	PHP_ABSTRACT_ME(Dao_Cache_BackendInterface, flush, arginfo_empty)
	PHP_FE_END
};

/**
 * Dao\Cache\BackendInterface initializer
 */
DAO_INIT_CLASS(Dao_Cache_BackendInterface){

	DAO_REGISTER_INTERFACE(Dao\\Cache, BackendInterface, cache_backendinterface, dao_cache_backendinterface_method_entry);

	return SUCCESS;
}

/**
 * Starts a cache. The $keyname allows to identify the created fragment
 *
 * @param string $keyName
 * @param long $lifetime
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, start);

/**
 * Stops the frontend without store any cached content
 *
 * @param boolean $stopBuffer
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, stop);

/**
 * Returns front-end instance adapter related to the back-end
 *
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, getFrontend);

/**
 * Returns the backend options
 *
 * @return array
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, getOptions);

/**
 * Checks whether the last cache is fresh or cached
 *
 * @return boolean
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, isFresh);

/**
 * Checks whether the cache has starting buffering or not
 *
 * @return boolean
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, isStarted);

/**
 * Returns a cached content
 *
 * @param string $keyName
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, get);

/**
 * Stores cached content into the file backend and stops the frontend
 *
 * @param string $keyName
 * @param string $content
 * @param long $lifetime
 * @param boolean $stopBuffer
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, save);

/**
 * Deletes a value from the cache by its key
 *
 * @param string $keyName
 * @return boolean
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, delete);

/**
 * Query the existing cached keys
 *
 * @param string $prefix
 * @return array
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, queryKeys);

/**
 * Checks if cache exists and it hasn't expired
 *
 * @param string $keyName
 * @return boolean
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, exists);

/**
 * Increment $key by $value
 *
 * @param string $key
 * @param mixed $value
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, increment);

/**
 * Decrement $key by $value
 *
 * @param string $key
 * @param mixed $value
 * @return mixed
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, decrement);

/**
 * Immediately invalidates all existing items.
 *
 * @return boolean
 */
DAO_DOC_METHOD(Dao_Cache_BackendInterface, flush);