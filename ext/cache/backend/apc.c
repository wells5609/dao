
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

#include "cache/backend/apc.h"
#include "cache/backend.h"
#include "cache/backendinterface.h"
#include "cache/exception.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/concat.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/exception.h"
#include "kernel/string.h"
#include "kernel/array.h"

/**
 * Dao\Cache\Backend\Apc
 *
 * Allows to cache output fragments, PHP data and raw data using an APC backend
 *
 *<code>
 *	//Cache data for 2 days
 *	$frontCache = new Dao\Cache\Frontend\Data(array(
 *		'lifetime' => 172800
 *	));
 *
 *  $cache = new Dao\Cache\Backend\Apc($frontCache, array(
 *      'prefix' => 'app-data'
 *  ));
 *
 *	//Cache arbitrary data
 *	$cache->save('my-data', array(1, 2, 3, 4, 5));
 *
 *	//Get data
 *	$data = $cache->get('my-data');
 *
 *</code>
 */
zend_class_entry *dao_cache_backend_apc_ce;

PHP_METHOD(Dao_Cache_Backend_Apc, get);
PHP_METHOD(Dao_Cache_Backend_Apc, save);
PHP_METHOD(Dao_Cache_Backend_Apc, delete);
PHP_METHOD(Dao_Cache_Backend_Apc, queryKeys);
PHP_METHOD(Dao_Cache_Backend_Apc, exists);
PHP_METHOD(Dao_Cache_Backend_Apc, increment);
PHP_METHOD(Dao_Cache_Backend_Apc, decrement);
PHP_METHOD(Dao_Cache_Backend_Apc, flush);

static const zend_function_entry dao_cache_backend_apc_method_entry[] = {
	PHP_ME(Dao_Cache_Backend_Apc, get, arginfo_dao_cache_backendinterface_get, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, save, arginfo_dao_cache_backendinterface_save, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, delete, arginfo_dao_cache_backendinterface_delete, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, queryKeys, arginfo_dao_cache_backendinterface_querykeys, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, exists, arginfo_dao_cache_backendinterface_exists, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, increment, arginfo_dao_cache_backendinterface_increment, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, decrement, arginfo_dao_cache_backendinterface_decrement, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Cache_Backend_Apc, flush, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static int dao_cache_backend_is_old_apcu = -1;

/**
 * Dao\Cache\Backend\Apc initializer
 */
DAO_INIT_CLASS(Dao_Cache_Backend_Apc){

	if (-1 == dao_cache_backend_is_old_apcu) {
		dao_cache_backend_is_old_apcu = zend_hash_str_exists(&module_registry, SL("apcu"));
		if (dao_cache_backend_is_old_apcu) {
			zend_constant *c;
			if ((c = zend_hash_str_find_ptr(EG(zend_constants), SL("APCU_APC_FULL_BC"))) != NULL) {
				dao_cache_backend_is_old_apcu = !zend_is_true(&c->value);
			}
		}
	}

	DAO_REGISTER_CLASS_EX(Dao\\Cache\\Backend, Apc, cache_backend_apc, dao_cache_backend_ce, dao_cache_backend_apc_method_entry, 0);

	zend_class_implements(dao_cache_backend_apc_ce, 1, dao_cache_backendinterface_ce);

	return SUCCESS;
}

/**
 * Returns a cached content
 *
 * @param string $keyName
 * @return mixed
 */
PHP_METHOD(Dao_Cache_Backend_Apc, get)
{
	zval *key_name, prefixed_key = {}, prefix = {}, frontend = {}, cached_content = {};

	dao_fetch_params(0, 1, 0, &key_name);

	dao_read_property(&prefix, getThis(), SL("_prefix"), PH_NOISY|PH_READONLY);

	DAO_CONCAT_SVV(&prefixed_key, "_PHCA", &prefix, key_name);

	DAO_CALL_FUNCTION(&cached_content, "apc_fetch", &prefixed_key);
	zval_ptr_dtor(&prefixed_key);
	if (DAO_IS_FALSE(&cached_content)) {
		RETURN_NULL();
	}

	if (dao_is_numeric(&cached_content)) {
		RETURN_ZVAL(&cached_content, 0, 0);
	} else {
		dao_read_property(&frontend, getThis(), SL("_frontend"), PH_NOISY|PH_READONLY);
		DAO_RETURN_CALL_METHOD(&frontend, "afterretrieve", &cached_content);
		zval_ptr_dtor(&cached_content);
	}
}

/**
 * Stores cached content into the APC backend and stops the frontend
 *
 * @param string $keyName
 * @param string $content
 * @param long $lifetime
 * @param boolean $stopBuffer
 */
PHP_METHOD(Dao_Cache_Backend_Apc, save)
{
	zval *key_name = NULL, *content = NULL, *lifetime = NULL, *stop_buffer = NULL, key ={}, prefix = {}, frontend = {};
	zval last_key = {}, cached_content = {}, prepared_content = {}, ttl = {}, is_buffering = {};

	dao_fetch_params(0, 0, 4, &key_name, &content, &lifetime, &stop_buffer);

	dao_read_property(&prefix, getThis(), SL("_prefix"), PH_NOISY|PH_READONLY);

	if (!key_name || Z_TYPE_P(key_name) == IS_NULL) {
		dao_read_property(&key, getThis(), SL("_lastKey"), PH_READONLY);
		key_name = &key;
	}

	if (!zend_is_true(key_name)) {
		DAO_THROW_EXCEPTION_STR(dao_cache_exception_ce, "The cache must be started first");
		return;
	}
	DAO_CONCAT_SVV(&last_key, "_PHCA", &prefix, key_name);

	dao_read_property(&frontend, getThis(), SL("_frontend"), PH_NOISY|PH_READONLY);
	if (!content || Z_TYPE_P(content) == IS_NULL) {
		DAO_CALL_METHOD(&cached_content, &frontend, "getcontent");
	} else {
		ZVAL_COPY(&cached_content, content);
	}

	if (!dao_is_numeric(&cached_content)) {
		DAO_CALL_METHOD(&prepared_content, &frontend, "beforestore", &cached_content);
	} else {
		ZVAL_COPY(&prepared_content, &cached_content);
	}

	/**
	 * Take the lifetime from the frontend or read it from the set in start()
	 */
	if (!lifetime || Z_TYPE_P(lifetime) != IS_LONG) {
		DAO_CALL_METHOD(&ttl, getThis(), "getlifetime");
	} else {
		ZVAL_COPY(&ttl, lifetime);
	}

	/**
	 * Call apc_store in the PHP userland since most of the time it isn't available at
	 * compile time
	 */
	DAO_CALL_FUNCTION(NULL, "apc_store", &last_key, &prepared_content, &ttl);
	zval_ptr_dtor(&last_key);
	zval_ptr_dtor(&ttl);
	zval_ptr_dtor(&prepared_content);

	DAO_CALL_METHOD(&is_buffering, &frontend, "isbuffering");
	if (!stop_buffer || DAO_IS_TRUE(stop_buffer)) {
		DAO_CALL_METHOD(NULL, &frontend, "stop");
	}

	if (DAO_IS_TRUE(&is_buffering)) {
		zend_print_zval(&cached_content, 0);
	}
	zval_ptr_dtor(&cached_content);

	dao_update_property_bool(getThis(), SL("_started"), 0);
}

/**
 * Increment of a given key, by number $value
 *
 * @param string $keyName
 * @param long $value
 * @return mixed
 */
PHP_METHOD(Dao_Cache_Backend_Apc, increment){

	zval *key_name, *value = NULL, prefix = {}, prefixed_key = {}, cached_content = {};

	dao_fetch_params(0, 1, 1, &key_name, &value);

	if (!value || Z_TYPE_P(value) == IS_NULL) {
		value = &DAO_GLOBAL(z_one);
	}

	dao_read_property(&prefix, getThis(), SL("_prefix"), PH_NOISY|PH_READONLY);

	DAO_CONCAT_SVV(&prefixed_key, "_PHCA", &prefix, key_name);

	if (SUCCESS == dao_function_exists_ex(SL("apc_inc"))) {
		DAO_RETURN_CALL_FUNCTION("apc_inc", &prefixed_key, value);
	} else {
		DAO_CALL_FUNCTION(&cached_content, "apc_fetch", &prefixed_key);

		if (Z_TYPE(cached_content) == IS_LONG) {
			add_function(return_value, &cached_content, value);
			DAO_CALL_METHOD(NULL, getThis(), "save", key_name, return_value);
		} else {
			RETVAL_FALSE;
		}
		zval_ptr_dtor(&cached_content);
	}
	zval_ptr_dtor(&prefixed_key);
}

/**
 * Decrement of a given key, by number $value
 *
 * @param string $keyName
 * @param long $value
 * @return mixed
 */
PHP_METHOD(Dao_Cache_Backend_Apc, decrement){

	zval *key_name, *value = NULL, prefix = {}, prefixed_key = {}, cached_content = {};

	dao_fetch_params(0, 1, 1, &key_name, &value);

	if (!value || Z_TYPE_P(value) == IS_NULL) {
		value = &DAO_GLOBAL(z_one);
	}

	dao_read_property(&prefix, getThis(), SL("_prefix"), PH_NOISY|PH_READONLY);

	DAO_CONCAT_SVV(&prefixed_key, "_PHCA", &prefix, key_name);

	if (SUCCESS == dao_function_exists_ex(SL("apc_dec"))) {
		DAO_RETURN_CALL_FUNCTION("apc_dec", &prefixed_key, value);
	} else {
		DAO_CALL_FUNCTION(&cached_content, "apc_fetch", &prefixed_key);

		if (Z_TYPE(cached_content) == IS_LONG) {
			dao_sub_function(return_value, &cached_content, value);
			DAO_CALL_METHOD(NULL, getThis(), "save", key_name, return_value);
		} else {
			RETVAL_FALSE;
		}
		zval_ptr_dtor(&cached_content);
	}
	zval_ptr_dtor(&prefixed_key);
}

/**
 * Deletes a value from the cache by its key
 *
 * @param string $keyName
 * @return boolean
 */
PHP_METHOD(Dao_Cache_Backend_Apc, delete){

	zval *key_name, prefix = {}, prefixed_key = {};

	dao_fetch_params(0, 1, 0, &key_name);

	dao_read_property(&prefix, getThis(), SL("_prefix"), PH_NOISY|PH_READONLY);

	DAO_CONCAT_SVV(&prefixed_key, "_PHCA", &prefix, key_name);

	DAO_RETURN_CALL_FUNCTION("apc_delete", &prefixed_key);
	zval_ptr_dtor(&prefixed_key);
}

/**
 * Query the existing cached keys
 *
 * @param string $prefix
 * @return array
 */
PHP_METHOD(Dao_Cache_Backend_Apc, queryKeys){

	zval *prefix = NULL, prefix_pattern = {}, iterator = {}, type = {};
	zend_class_entry *apciterator_ce;
	zend_object_iterator *it;

	dao_fetch_params(0, 0, 1, &prefix);

	if (!prefix || Z_TYPE_P(prefix) == IS_NULL) {
		ZVAL_STRING(&prefix_pattern, "/^_PHCA/");
	} else {
		DAO_CONCAT_SVS(&prefix_pattern, "/^_PHCA", prefix, "/");
	}

	array_init(return_value);

	apciterator_ce = dao_fetch_str_class(SL("APCIterator"), ZEND_FETCH_CLASS_AUTO);

	object_init_ex(&iterator, apciterator_ce);
	assert(dao_has_constructor(&iterator));
	if (!dao_cache_backend_is_old_apcu) {
		ZVAL_STRING(&type, "user");
		DAO_CALL_METHOD(NULL, &iterator, "__construct", &type, &prefix_pattern);
		zval_ptr_dtor(&type);
	}else {
		DAO_CALL_METHOD(NULL, &iterator, "__construct", &prefix_pattern);
	}
	zval_ptr_dtor(&prefix_pattern);

	/* APCIterator implements Iterator */
	assert(instanceof_function(apciterator_ce, zend_ce_iterator));

	it = apciterator_ce->get_iterator(apciterator_ce, &iterator, 0);

	/* APCIterator is an iterator */
	assert(it != NULL);

	/* APCIterator has key() method */
	assert(it->funcs->get_current_key != NULL);

	/* APCIterator has rewind() method */
	assert(it->funcs->rewind != NULL);

	it->funcs->rewind(it);
	while (it->funcs->valid(it) == SUCCESS && !EG(exception)) {
		zval key = {}, itkey = {};

		it->funcs->get_current_key(it, &itkey);
		if (likely(Z_TYPE(itkey) == IS_STRING)) {
			ZVAL_STRINGL(&key, Z_STRVAL(itkey) + 5, Z_STRLEN(itkey) - 5);
			dao_array_append(return_value, &key, 0);
		}

		it->funcs->move_forward(it);
	}

	if (it) {
		zend_iterator_dtor(it);
	}
	zval_ptr_dtor(&iterator);
}

/**
 * Checks if cache exists and it hasn't expired
 *
 * @param string $keyName
 * @return boolean
 */
PHP_METHOD(Dao_Cache_Backend_Apc, exists){

	zval *key_name, prefix = {}, prefixed_key = {}, cache_exists = {};

	dao_fetch_params(0, 1, 0, &key_name);

	dao_read_property(&prefix, getThis(), SL("_prefix"), PH_READONLY);
	DAO_CONCAT_SVV(&prefixed_key, "_PHCA", &prefix, key_name);

	DAO_CALL_FUNCTION(&cache_exists, "apc_exists", &prefixed_key);
	if (DAO_IS_NOT_FALSE(&cache_exists)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zval_ptr_dtor(&prefixed_key);
}

/**
 * Immediately invalidates all existing items.
 *
 * @return boolean
 */
PHP_METHOD(Dao_Cache_Backend_Apc, flush){

	zval prefix_pattern = {}, iterator = {}, type = {};
	zend_class_entry *apciterator_ce;
	zend_object_iterator *it;

	ZVAL_STRING(&prefix_pattern, "/^_PHCA/");

	apciterator_ce = dao_fetch_str_class(SL("APCIterator"), ZEND_FETCH_CLASS_AUTO);

	object_init_ex(&iterator, apciterator_ce);
	assert(dao_has_constructor(&iterator));
	if (!dao_cache_backend_is_old_apcu) {
		ZVAL_STRING(&type, "user");
		DAO_CALL_METHOD(NULL, &iterator, "__construct", &type, &prefix_pattern);
		zval_ptr_dtor(&type);
	} else {
		DAO_CALL_METHOD(NULL, &iterator, "__construct", &prefix_pattern);
	}
	zval_ptr_dtor(&prefix_pattern);

	/* APCIterator implements Iterator */
	assert(instanceof_function(apciterator_ce, zend_ce_iterator));

	it = apciterator_ce->get_iterator(apciterator_ce, &iterator, 0);

	/* APCIterator is an iterator */
	assert(it != NULL);

	/* APCIterator has key() method */
	assert(it->funcs->get_current_key != NULL);

	/* APCIterator has rewind() method */
	assert(it->funcs->rewind != NULL);

	it->funcs->rewind(it);
	while (it->funcs->valid(it) == SUCCESS && !EG(exception)) {
		zval itkey = {};
		int flag;

		it->funcs->get_current_key(it, &itkey);
		
		if (EG(exception)) {
			continue;
		}
		if (likely(Z_TYPE(itkey) == IS_STRING)) {
			DAO_CALL_FUNCTION_FLAG(flag, NULL, "apc_delete", &itkey);
			if (FAILURE == flag) {
				break;
			}
		}
		it->funcs->move_forward(it);
	}

	if (it) {
		zend_iterator_dtor(it);
	}
	zval_ptr_dtor(&iterator);
	RETURN_TRUE;
}
