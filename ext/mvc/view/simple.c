
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

#include "mvc/view/simple.h"
#include "mvc/view/exception.h"
#include "mvc/view/engineinterface.h"
#include "mvc/view/engine/php.h"
#include "cache/backendinterface.h"
#include "di/injectable.h"
#include "debug.h"

#include <Zend/zend_closures.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/operators.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/hash.h"
#include "kernel/concat.h"
#include "kernel/file.h"
#include "kernel/string.h"
#include "kernel/output.h"
#include "kernel/debug.h"

/**
 * Dao\Mvc\View\Simple
 *
 * This component allows to render views without hicherquical levels
 *
 *<code>
 * $view = new Dao\Mvc\View\Simple();
 * echo $view->render('templates/my-view', array('content' => $html));
 *</code>
 *
 */
zend_class_entry *dao_mvc_view_simple_ce;

PHP_METHOD(Dao_Mvc_View_Simple, __construct);
PHP_METHOD(Dao_Mvc_View_Simple, setViewsDir);
PHP_METHOD(Dao_Mvc_View_Simple, getViewsDir);
PHP_METHOD(Dao_Mvc_View_Simple, registerEngines);
PHP_METHOD(Dao_Mvc_View_Simple, getRegisteredEngines);
PHP_METHOD(Dao_Mvc_View_Simple, _loadTemplateEngines);
PHP_METHOD(Dao_Mvc_View_Simple, _internalRender);
PHP_METHOD(Dao_Mvc_View_Simple, render);
PHP_METHOD(Dao_Mvc_View_Simple, partial);
PHP_METHOD(Dao_Mvc_View_Simple, setCacheOptions);
PHP_METHOD(Dao_Mvc_View_Simple, getCacheOptions);
PHP_METHOD(Dao_Mvc_View_Simple, _createCache);
PHP_METHOD(Dao_Mvc_View_Simple, getCache);
PHP_METHOD(Dao_Mvc_View_Simple, cache);
PHP_METHOD(Dao_Mvc_View_Simple, setParamToView);
PHP_METHOD(Dao_Mvc_View_Simple, setVars);
PHP_METHOD(Dao_Mvc_View_Simple, setVar);
PHP_METHOD(Dao_Mvc_View_Simple, getParamsToView);
PHP_METHOD(Dao_Mvc_View_Simple, setContent);
PHP_METHOD(Dao_Mvc_View_Simple, getContent);
PHP_METHOD(Dao_Mvc_View_Simple, startSection);
PHP_METHOD(Dao_Mvc_View_Simple, stopSection);
PHP_METHOD(Dao_Mvc_View_Simple, section);
PHP_METHOD(Dao_Mvc_View_Simple, getActiveRenderPath);
PHP_METHOD(Dao_Mvc_View_Simple, __set);
PHP_METHOD(Dao_Mvc_View_Simple, __get);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_setviewsdir, 0, 0, 1)
	ZEND_ARG_INFO(0, viewsDir)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_registerengines, 0, 0, 1)
	ZEND_ARG_INFO(0, engines)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_render, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, params)
	ZEND_ARG_INFO(0, absolutePath)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_partial, 0, 0, 1)
	ZEND_ARG_INFO(0, partialPath)
	ZEND_ARG_INFO(0, params)
	ZEND_ARG_INFO(0, absolutePath)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_setcacheoptions, 0, 0, 1)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_cache, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_setparamtoview, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_setvars, 0, 0, 1)
	ZEND_ARG_INFO(0, params)
	ZEND_ARG_INFO(0, merge)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_setvar, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_setcontent, 0, 0, 1)
	ZEND_ARG_INFO(0, content)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_startsection, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple_section, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, defaultValue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple___set, 0, 0, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_view_simple___get, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_mvc_view_simple_method_entry[] = {
	PHP_ME(Dao_Mvc_View_Simple, __construct, arginfo_dao_mvc_view_simple___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Mvc_View_Simple, setViewsDir, arginfo_dao_mvc_view_simple_setviewsdir, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, getViewsDir, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, registerEngines, arginfo_dao_mvc_view_simple_registerengines, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, getRegisteredEngines, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, _loadTemplateEngines, arginfo_empty, ZEND_ACC_PROTECTED)
	PHP_ME(Dao_Mvc_View_Simple, _internalRender, arginfo_empty, ZEND_ACC_PROTECTED)
	PHP_ME(Dao_Mvc_View_Simple, render, arginfo_dao_mvc_view_simple_render, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, partial, arginfo_dao_mvc_view_simple_partial, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, setCacheOptions, arginfo_dao_mvc_view_simple_setcacheoptions, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, getCacheOptions, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, _createCache, arginfo_empty, ZEND_ACC_PROTECTED)
	PHP_ME(Dao_Mvc_View_Simple, getCache, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, cache, arginfo_dao_mvc_view_simple_cache, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, setParamToView, arginfo_dao_mvc_view_simple_setparamtoview, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, setVars, arginfo_dao_mvc_view_simple_setvars, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, setVar, arginfo_dao_mvc_view_simple_setvar, ZEND_ACC_PUBLIC)
	PHP_MALIAS(Dao_Mvc_View_Simple, getVar, __get, arginfo_dao_mvc_view_simple___get, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, getParamsToView, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, setContent, arginfo_dao_mvc_view_simple_setcontent, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, getContent, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, startSection, arginfo_dao_mvc_view_simple_startsection, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, stopSection, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, section, arginfo_dao_mvc_view_simple_section, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, getActiveRenderPath, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, __set, arginfo_dao_mvc_view_simple___set, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_View_Simple, __get, arginfo_dao_mvc_view_simple___get, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Mvc\View\Simple initializer
 */
DAO_INIT_CLASS(Dao_Mvc_View_Simple){

	DAO_REGISTER_CLASS_EX(Dao\\Mvc\\View, Simple, mvc_view_simple, dao_di_injectable_ce, dao_mvc_view_simple_method_entry, 0);

	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_options"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_viewsDir"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_partialsDir"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_viewParams"), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(dao_mvc_view_simple_ce, SL("_engines"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_registeredEngines"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_activeRenderPath"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_content"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_sections"), ZEND_ACC_PROTECTED);
	zend_declare_property_bool(dao_mvc_view_simple_ce, SL("_cache"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_mvc_view_simple_ce, SL("_cacheOptions"), ZEND_ACC_PROTECTED);

	return SUCCESS;
}

/**
 * Dao\Mvc\View constructor
 *
 * @param array $options
 */
PHP_METHOD(Dao_Mvc_View_Simple, __construct){

	zval *options = NULL;

	dao_fetch_params(0, 0, 1, &options);

	if (options && Z_TYPE_P(options) == IS_ARRAY) {
		dao_update_property(getThis(), SL("_options"), options);
	}
	dao_update_property_empty_array(getThis(), SL("_sections"));
}

/**
 * Sets views directory. Depending of your platform, always add a trailing slash or backslash
 *
 * @param string|array $viewsDir
 */
PHP_METHOD(Dao_Mvc_View_Simple, setViewsDir){

	zval *views_dir;

	dao_fetch_params(0, 1, 0, &views_dir);

	dao_update_property(getThis(), SL("_viewsDir"), views_dir);

}

/**
 * Gets views directory
 *
 * @return string
 */
PHP_METHOD(Dao_Mvc_View_Simple, getViewsDir){


	RETURN_MEMBER(getThis(), "_viewsDir");
}

/**
 * Register templating engines
 *
 *<code>
 *$this->view->registerEngines(array(
 *  ".phtml" => "Dao\Mvc\View\Engine\Php",
 *  ".mhtml" => "MyCustomEngine"
 *));
 *</code>
 *
 * @param array $engines
 */
PHP_METHOD(Dao_Mvc_View_Simple, registerEngines){

	zval *engines;

	dao_fetch_params(0, 1, 0, &engines);

	if (Z_TYPE_P(engines) != IS_ARRAY) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_view_exception_ce, "Engines to register must be an array");
		return;
	}
	dao_update_property(getThis(), SL("_registeredEngines"), engines);

}

/**
 * Returns the registered templating engines
 *
 * @brief array Dao\Mvc\View\Simple::getRegisteredEngines()
 */
PHP_METHOD(Dao_Mvc_View_Simple, getRegisteredEngines) {

	RETURN_MEMBER(getThis(), "_registeredEngines")
}

/**
 * Loads registered template engines, if none is registered it will use Dao\Mvc\View\Engine\Php
 *
 * @return array
 */
PHP_METHOD(Dao_Mvc_View_Simple, _loadTemplateEngines){

	zval engines = {}, dependency_injector = {}, registered_engines = {}, php_engine = {}, arguments = {}, *engine_service;
	zend_string *str_key;
	ulong idx;

	DAO_MM_INIT();
	dao_read_property(&engines, getThis(), SL("_engines"), PH_READONLY);

	/**
	 * If the engines aren't initialized 'engines' is false
	 */
	if (!zend_is_true(&engines)) {
		
		DAO_MM_CALL_METHOD(&dependency_injector, getThis(), "getdi");
		DAO_MM_ADD_ENTRY(&dependency_injector);
		if (Z_TYPE(dependency_injector) != IS_OBJECT) {
			DAO_MM_THROW_EXCEPTION_STR(dao_mvc_view_exception_ce, "A dependency injector container is required to obtain the application services");
			return;
		}

		array_init(&engines);
		DAO_MM_ADD_ENTRY(&engines);
		dao_read_property(&registered_engines, getThis(), SL("_registeredEngines"), PH_NOISY|PH_READONLY);
		if (Z_TYPE(registered_engines) != IS_ARRAY) {
			/**
			 * We use Dao\Mvc\View\Engine\Php as default
			 */
			object_init_ex(&php_engine, dao_mvc_view_engine_php_ce);
			DAO_MM_ADD_ENTRY(&php_engine);
			DAO_MM_CALL_METHOD(NULL, &php_engine, "__construct", getThis(), &dependency_injector);

			/**
			 * Use .phtml as extension for the PHP engine
			 */
			dao_array_update_str(&engines, SL(".phtml"), &php_engine, PH_COPY);
		} else {
			/**
			 * Arguments for instantiated engines
			 */
			array_init_size(&arguments, 2);
			DAO_MM_ADD_ENTRY(&arguments);
			dao_array_append(&arguments, getThis(), PH_COPY);
			dao_array_append(&arguments, &dependency_injector, PH_COPY);

			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(registered_engines), idx, str_key, engine_service) {
				zval extension = {}, engine_object = {}, exception_message = {};
				if (str_key) {
					ZVAL_STR(&extension, str_key);
				} else {
					ZVAL_LONG(&extension, idx);
				}
				if (Z_TYPE_P(engine_service) == IS_OBJECT) {

					/**
					 * Engine can be a closure
					 */
					if (instanceof_function(Z_OBJCE_P(engine_service), zend_ce_closure)) {
						DAO_MM_CALL_USER_FUNC_ARRAY(&engine_object, engine_service, &arguments);
						DAO_MM_ADD_ENTRY(&engine_object);
					} else {
						ZVAL_COPY_VALUE(&engine_object, engine_service);
					}
				} else if (Z_TYPE_P(engine_service) == IS_STRING) {
					/**
					 * Engine can be a string representing a service in the DI
					 */
					DAO_MM_CALL_METHOD(&engine_object, &dependency_injector, "getshared", engine_service, &arguments);
					DAO_MM_VERIFY_INTERFACE(&engine_object, dao_mvc_view_engineinterface_ce);
				} else {
					DAO_CONCAT_SV(&exception_message, "Invalid template engine registration for extension: ", &extension);
					DAO_MM_ADD_ENTRY(&exception_message);
					DAO_MM_THROW_EXCEPTION_ZVAL(dao_mvc_view_exception_ce, &exception_message);
					return;
				}
				dao_array_update(&engines, &extension, &engine_object, PH_COPY);
			} ZEND_HASH_FOREACH_END();
		}

		dao_update_property(getThis(), SL("_engines"), &engines);
	}

	RETURN_MM_CTOR(&engines);
}

/**
 * Tries to render the view with every engine registered in the component
 *
 * @param string $path
 * @param array $params
 */
PHP_METHOD(Dao_Mvc_View_Simple, _internalRender){

	zval *view_path, *params, *absolute_path = NULL, event_name = {}, status = {}, debug_message = {};
	zval not_exists = {}, views_dir = {}, views_dir_paths = {}, *path = NULL, engines = {}, *engine, exception_message = {};
	zend_string *str_key;
	ulong idx;

	dao_fetch_params(1, 2, 0, &view_path, &params, &absolute_path);

	if (absolute_path == NULL) {
		absolute_path = &DAO_GLOBAL(z_false);
	}

	dao_update_property(getThis(), SL("_activeRenderPath"), view_path);

	/**
	 * Call beforeRender if there is an events manager
	 */
	DAO_MM_ZVAL_STRING(&event_name, "view:beforeRender");
	DAO_MM_CALL_METHOD(&status, getThis(), "fireeventcancel", &event_name);
	if (DAO_IS_FALSE(&status)) {
		RETURN_NULL();
	}
	zval_ptr_dtor(&status);

	if (unlikely(DAO_GLOBAL(debug).enable_debug)) {
		DAO_CONCAT_SV(&debug_message, "Render Simple View: ", view_path);
		DAO_DEBUG_LOG(&debug_message);
		zval_ptr_dtor(&debug_message);
	}

	ZVAL_TRUE(&not_exists);
	array_init(&views_dir_paths);
	DAO_MM_ADD_ENTRY(&views_dir_paths);
	if (zend_is_true(absolute_path)) {
		dao_array_append(&views_dir_paths, view_path, PH_COPY);
	} else {
		dao_read_property(&views_dir, getThis(), SL("_viewsDir"), PH_NOISY|PH_READONLY);
		if (Z_TYPE(views_dir) == IS_ARRAY) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL(views_dir), path) {
				zval views_dir_path = {};
				DAO_CONCAT_VV(&views_dir_path, path, view_path);
				dao_array_append(&views_dir_paths, &views_dir_path, 0);
			} ZEND_HASH_FOREACH_END();
		} else {
			zval views_dir_path = {};
			DAO_CONCAT_VV(&views_dir_path, &views_dir, view_path);
			dao_array_append(&views_dir_paths, &views_dir_path, 0);
		}
	}

	/**
	 * Load the template engines
	 */
	DAO_MM_CALL_METHOD(&engines, getThis(), "_loadtemplateengines");
	DAO_MM_ADD_ENTRY(&engines);

	/**
	 * Views are rendered in each engine
	 */
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(engines), idx, str_key, engine) {
		zval extension = {}, view_engine_path = {};
		if (str_key) {
			ZVAL_STR(&extension, str_key);
		} else {
			ZVAL_LONG(&extension, idx);
		}
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(views_dir_paths), path) {
				DAO_CONCAT_VV(&view_engine_path, path, &extension);

			if (dao_file_exists(&view_engine_path) == SUCCESS) {

				if (unlikely(DAO_GLOBAL(debug).enable_debug)) {
					DAO_CONCAT_SV(&debug_message, "--Found: ", &view_engine_path);
					DAO_DEBUG_LOG(&debug_message);
					zval_ptr_dtor(&debug_message);
				}

				/**
				 * Call beforeRenderView if there is a events manager available
				 */
				DAO_MM_ZVAL_STRING(&event_name, "view:beforeRenderView");
				DAO_MM_CALL_METHOD(&status, getThis(), "fireeventcancel", &event_name, &view_engine_path);
				if (DAO_IS_FALSE(&status)) {
					continue;
				}
				zval_ptr_dtor(&status);

				DAO_MM_CALL_METHOD(NULL, engine, "render", &view_engine_path, params, &DAO_GLOBAL(z_true));

				/**
				 * Call afterRenderView if there is a events manager available
				 */
				ZVAL_FALSE(&not_exists);
				DAO_MM_ZVAL_STRING(&event_name, "view:afterRenderView");
				DAO_MM_CALL_METHOD(NULL, getThis(), "fireevent", &event_name);
				break;
			} else if (unlikely(DAO_GLOBAL(debug).enable_debug)) {
				DAO_CONCAT_SV(&debug_message, "--Not Found: ", &view_engine_path);
				DAO_DEBUG_LOG(&debug_message);
				zval_ptr_dtor(&debug_message);
			}
		} ZEND_HASH_FOREACH_END();

		if (!zend_is_true(&not_exists)) {
			break;
		}
	} ZEND_HASH_FOREACH_END();

	/**
	 * Always throw an exception if the view does not exist
	 */
	if (DAO_IS_TRUE(&not_exists)) {
		DAO_CONCAT_SVS(&exception_message, "View '", view_path, "' was not found in the views directory");
		DAO_MM_ADD_ENTRY(&exception_message);
		DAO_MM_THROW_EXCEPTION_ZVAL(dao_mvc_view_exception_ce, &exception_message);
		return;
	}

	/**
	 * Call afterRender event
	 */
	DAO_MM_ZVAL_STRING(&event_name, "view:afterRender");
	DAO_MM_CALL_METHOD(NULL, getThis(), "fireevent", &event_name);
	RETURN_MM();
}

/**
 * Renders a view
 *
 * @param string $path
 * @param array $params
 * @return string
 */
PHP_METHOD(Dao_Mvc_View_Simple, render){

	zval *path, *params = NULL, *absolute_path = NULL, cache = {}, is_started = {}, key = {}, lifetime = {}, cache_options = {}, content = {}, view_params = {};
	zval merged_params = {};
	int flag = 0;

	dao_fetch_params(1, 1, 1, &path, &params, &absolute_path);

	if (!params) {
		params = &DAO_GLOBAL(z_null);
	}

	if (absolute_path == NULL) {
		absolute_path = &DAO_GLOBAL(z_false);
	}

	/**
	 * Create/Get a cache
	 */
	DAO_MM_CALL_METHOD(&cache, getThis(), "getcache");
	DAO_MM_ADD_ENTRY(&cache);
	if (Z_TYPE(cache) == IS_OBJECT) {
		/**
		 * Check if the cache is started, the first time a cache is started we start the cache
		 */
		DAO_MM_CALL_METHOD(&is_started, &cache, "isstarted");
		DAO_MM_ADD_ENTRY(&is_started);
		if (DAO_IS_FALSE(&is_started)) {
			dao_read_property(&cache_options, getThis(), SL("_cacheOptions"), PH_NOISY|PH_READONLY);

			/**
			 * Check if the user has defined a different options to the default
			 */
			if (Z_TYPE(cache_options) == IS_ARRAY) {
				if (!dao_array_isset_fetch_str(&key, &cache_options, SL("key"), PH_READONLY)) {
					ZVAL_NULL(&key);
				}
				if (dao_array_isset_str(&cache_options, SL("lifetime"))) {
					dao_array_fetch_str(&lifetime, &cache_options, SL("lifetime"), PH_NOISY|PH_READONLY);
				} else {
					ZVAL_NULL(&lifetime);
				}
			}

			/**
			 * If a cache key is not set we create one using a md5
			 */
			if (Z_TYPE(key) <= IS_NULL) {
				dao_md5(&key, path);
				DAO_MM_ADD_ENTRY(&key);
			}

			/**
			 * We start the cache using the key set
			 */
			DAO_MM_CALL_METHOD(&content, &cache, "start", &key, &lifetime, &DAO_GLOBAL(z_true));
			DAO_MM_ADD_ENTRY(&content);
			if (Z_TYPE(content) != IS_NULL) {
				dao_update_property(getThis(), SL("_content"), &content);
				RETURN_MM_CTOR(&content);
			}
		}
	}

	dao_read_property(&view_params, getThis(), SL("_viewParams"), PH_NOISY|PH_READONLY);

	/**
	 * Merge parameters
	 */
	if (Z_TYPE_P(params) == IS_ARRAY) {
		if (Z_TYPE(view_params) == IS_ARRAY) {
			dao_fast_array_merge(&merged_params, &view_params, params);
			DAO_MM_ADD_ENTRY(&merged_params);
		} else {
			ZVAL_COPY_VALUE(&merged_params, params);
		}
	} else {
		ZVAL_COPY_VALUE(&merged_params, &view_params);
	}

	dao_ob_start();

	/**
	 * internalRender is also reused by partials
	 */
	DAO_CALL_METHOD_FLAG(flag, NULL, getThis(), "_internalrender", path, &merged_params, absolute_path);
	
	dao_ob_end_clean();
	if (flag == FAILURE) {
		return;
	}

	DAO_MM_CALL_METHOD(&content, getThis(), "getcontent");
	DAO_MM_ADD_ENTRY(&content);

	/**
	 * Store the data in output into the cache
	 */
	if (Z_TYPE(cache) == IS_OBJECT) {
		DAO_MM_CALL_METHOD(NULL, &cache, "save", &DAO_GLOBAL(z_null), &content);
	}

	RETURN_MM_CTOR(&content);
}

/**
 * Renders a partial view
 *
 * <code>
 * 	//Show a partial inside another view
 * 	$this->partial('shared/footer');
 * </code>
 *
 * <code>
 * 	//Show a partial inside another view with parameters
 * 	$this->partial('shared/footer', array('content' => $html));
 * </code>
 *
 * @param string $partialPath
 * @param array $params
 */
PHP_METHOD(Dao_Mvc_View_Simple, partial){

	zval *partial_path, *params = NULL, *absolute_path = NULL, view_params = {}, merged_params = {}, content = {};

	dao_fetch_params(1, 1, 1, &partial_path, &params, &absolute_path);

	if (!params) {
		params = &DAO_GLOBAL(z_null);
	}

	if (absolute_path == NULL) {
		absolute_path = &DAO_GLOBAL(z_false);
	}

	/**
	 * Start ouput buffering
	 */
	dao_ob_start();

	/**
	 * If the developer pass an array of variables we create a new virtual symbol table
	 */
	dao_read_property(&view_params, getThis(), SL("_viewParams"), PH_NOISY|PH_READONLY);
	if (Z_TYPE_P(params) == IS_ARRAY) {
		/**
		 * Merge or assign the new params as parameters
		 */
		if (Z_TYPE(view_params) == IS_ARRAY) {
			dao_fast_array_merge(&merged_params, &view_params, params);
			DAO_MM_ADD_ENTRY(&merged_params);
		} else {
			ZVAL_COPY_VALUE(&merged_params, params);
		}
	} else {
		ZVAL_COPY_VALUE(&merged_params, &view_params);
	}

	/**
	 * Call engine render, this checks in every registered engine for the partial
	 */
	DAO_MM_CALL_METHOD(NULL, getThis(), "_internalrender", partial_path, &merged_params, absolute_path);

	dao_ob_end_clean();

	dao_read_property(&content, getThis(), SL("_content"), PH_NOISY|PH_READONLY);

	/**
	 * Content is output to the parent view
	 */
	zend_print_zval(&content, 0);
	RETURN_MM();
}

/**
 * Sets the cache options
 *
 * @param array $options
 * @return Dao\Mvc\View\Simple
 */
PHP_METHOD(Dao_Mvc_View_Simple, setCacheOptions){

	zval *options;

	dao_fetch_params(0, 1, 0, &options);

	dao_update_property(getThis(), SL("_cacheOptions"), options);
	RETURN_THIS();
}

/**
 * Returns the cache options
 *
 * @return array
 */
PHP_METHOD(Dao_Mvc_View_Simple, getCacheOptions){


	RETURN_MEMBER(getThis(), "_cacheOptions");
}

/**
 * Create a Dao\Cache based on the internal cache options
 *
 * @return Dao\Cache\BackendInterface
 */
PHP_METHOD(Dao_Mvc_View_Simple, _createCache){

	zval dependency_injector = {}, cache_service = {}, cache_options = {};

	DAO_MM_INIT();
	DAO_MM_CALL_METHOD(&dependency_injector, getThis(), "getdi");
	DAO_MM_ADD_ENTRY(&dependency_injector);
	if (Z_TYPE(dependency_injector) != IS_OBJECT) {
		DAO_MM_THROW_EXCEPTION_STR(dao_mvc_view_exception_ce, "A dependency injector container is required to obtain the view cache services");
		return;
	}

	dao_read_property(&cache_options, getThis(), SL("_cacheOptions"), PH_NOISY|PH_READONLY);
	if (Z_TYPE(cache_options) != IS_ARRAY ||
		!dao_array_isset_fetch_str(&cache_service, &cache_options, SL("service"), PH_READONLY)) {
		DAO_MM_ZVAL_STRING(&cache_service, "viewCache");
	}

	/**
	 * The injected service must be an object
	 */
	DAO_MM_CALL_METHOD(return_value, &dependency_injector, "getshared", &cache_service);

	if (Z_TYPE_P(return_value) != IS_OBJECT) {
		DAO_MM_THROW_EXCEPTION_STR(dao_mvc_view_exception_ce, "The injected caching service is invalid");
		return;
	}

	DAO_MM_VERIFY_INTERFACE(return_value, dao_cache_backendinterface_ce);
	RETURN_MM();
}

/**
 * Returns the cache instance used to cache
 *
 * @return Dao\Cache\BackendInterface
 */
PHP_METHOD(Dao_Mvc_View_Simple, getCache){


	dao_read_property(return_value, getThis(), SL("_cache"), PH_COPY);

	if (Z_TYPE_P(return_value) == IS_TRUE) {
		DAO_CALL_METHOD(return_value, getThis(), "_createcache");
		dao_update_property(getThis(), SL("_cache"), return_value);
	}
}

/**
 * Cache the actual view render to certain level
 *
 *<code>
 *  $this->view->cache(array('key' => 'my-key', 'lifetime' => 86400));
 *</code>
 *
 * @param boolean|array $options
 * @return Dao\Mvc\View\Simple
 */
PHP_METHOD(Dao_Mvc_View_Simple, cache){

	zval *options = NULL;

	dao_fetch_params(0, 0, 1, &options);

	if (!options) {
		options = &DAO_GLOBAL(z_true);
	}

	if (Z_TYPE_P(options) == IS_ARRAY) {
		dao_update_property(getThis(), SL("_cache"), &DAO_GLOBAL(z_true));
		dao_update_property(getThis(), SL("_cacheOptions"), options);
	} else {
		if (zend_is_true(options)) {
			dao_update_property(getThis(), SL("_cache"), &DAO_GLOBAL(z_true));
		} else {
			dao_update_property(getThis(), SL("_cache"), &DAO_GLOBAL(z_false));
		}
	}

	RETURN_THIS();
}

/**
 * Adds parameters to views (alias of setVar)
 *
 *<code>
 *	$this->view->setParamToView('products', $products);
 *</code>
 *
 * @param string $key
 * @param mixed $value
 * @return Dao\Mvc\View\Simple
 */
PHP_METHOD(Dao_Mvc_View_Simple, setParamToView){

	zval *key, *value;

	dao_fetch_params(0, 2, 0, &key, &value);

	dao_update_property_array(getThis(), SL("_viewParams"), key, value);
	RETURN_THIS();
}

/**
 * Set all the render params
 *
 *<code>
 *	$this->view->setVars(array('products' => $products));
 *</code>
 *
 * @param array $params
 * @param boolean $merge
 * @return Dao\Mvc\View\Simple
 */
PHP_METHOD(Dao_Mvc_View_Simple, setVars){

	zval *params, *merge = NULL, view_params = {};

	dao_fetch_params(0, 1, 1, &params, &merge);
	DAO_SEPARATE_PARAM(params);

	if (!merge) {
		merge = &DAO_GLOBAL(z_true);
	}

	if (Z_TYPE_P(params) != IS_ARRAY) {
		DAO_MM_THROW_EXCEPTION_STR(dao_mvc_view_exception_ce, "The render parameters must be an array");
		return;
	}

	if (zend_is_true(merge)) {
		dao_read_property(&view_params, getThis(), SL("_viewParams"), PH_NOISY|PH_READONLY);
		if (Z_TYPE(view_params) == IS_ARRAY) {
			dao_array_replace(&view_params, params);
		} else {
			dao_update_property(getThis(), SL("_viewParams"), params);
		}

	} else {
		dao_update_property(getThis(), SL("_viewParams"), params);
	}

	RETURN_THIS();
}

/**
 * Set a single view parameter
 *
 *<code>
 *	$this->view->setVar('products', $products);
 *</code>
 *
 * @param string $key
 * @param mixed $value
 * @return Dao\Mvc\View\Simple
 */
PHP_METHOD(Dao_Mvc_View_Simple, setVar){

	zval *key, *value;

	dao_fetch_params(0, 2, 0, &key, &value);

	dao_update_property_array(getThis(), SL("_viewParams"), key, value);
	RETURN_THIS();
}

/**
 * Returns a parameter previously set in the view
 *
 * @param string $key
 * @return mixed
 * @see __get()
 */
DAO_DOC_METHOD(Dao_Mvc_View_Simple, getVar);

/**
 * Returns parameters to views
 *
 * @return array
 */
PHP_METHOD(Dao_Mvc_View_Simple, getParamsToView){


	RETURN_MEMBER(getThis(), "_viewParams");
}

/**
 * Externally sets the view content
 *
 *<code>
 *	$this->view->setContent("<h1>hello</h1>");
 *</code>
 *
 * @param string $content
 * @return Dao\Mvc\View\Simple
 */
PHP_METHOD(Dao_Mvc_View_Simple, setContent){

	zval *content;

	dao_fetch_params(0, 1, 0, &content);

	if (Z_TYPE_P(content) != IS_STRING) {
		DAO_THROW_EXCEPTION_STR(dao_mvc_view_exception_ce, "Content must be a string");
		return;
	}
	dao_update_property(getThis(), SL("_content"), content);

	RETURN_THIS();
}

/**
 * Returns cached ouput from another view stage
 *
 * @return string
 */
PHP_METHOD(Dao_Mvc_View_Simple, getContent){


	RETURN_MEMBER(getThis(), "_content");
}

/**
 * Start a new section block
 *
 * @param string $name
 */
PHP_METHOD(Dao_Mvc_View_Simple, startSection){

	zval *name;

	dao_fetch_params(0, 1, 0, &name);

	dao_update_property_array(getThis(), SL("_sections"), name, &DAO_GLOBAL(z_null));
	dao_ob_start();
	RETURN_THIS();
}

/**
 * Stop the current section block
 *
 * @return string
 */
PHP_METHOD(Dao_Mvc_View_Simple, stopSection){

	zval content = {}, sections = {}, name = {};
	HashTable *array;

	dao_ob_get_clean(&content);

	dao_read_property(&sections, getThis(), SL("_sections"), PH_NOISY|PH_READONLY);

	array = Z_ARRVAL(sections);

	zend_hash_internal_pointer_end(array);
	zend_hash_get_current_key_zval(array, &name);

	dao_update_property_array(getThis(), SL("_sections"), &name, &content);
	zval_ptr_dtor(&content);
	RETURN_THIS();
}

/**
 * Returns the content for a section block
 *
 * @param string $name
 * @param string $default
 * @return string|null
 */
PHP_METHOD(Dao_Mvc_View_Simple, section){

	zval *name, *default_value = NULL;

	dao_fetch_params(0, 1, 1, &name, &default_value);

	if (dao_isset_property_array(getThis(), SL("_sections"), name)) {
		dao_read_property_array(return_value, getThis(), SL("_sections"), name, PH_COPY);
	} else if (default_value) {
		RETURN_CTOR(default_value);
	} else {
		RETURN_NULL();
	}
}

/**
 * Returns the path of the view that is currently rendered
 *
 * @return string
 */
PHP_METHOD(Dao_Mvc_View_Simple, getActiveRenderPath){


	RETURN_MEMBER(getThis(), "_activeRenderPath");
}

/**
 * Magic method to pass variables to the views
 *
 *<code>
 *	$this->view->products = $products;
 *</code>
 *
 * @param string $key
 * @param mixed $value
 */
PHP_METHOD(Dao_Mvc_View_Simple, __set){

	zval *key, *value;

	dao_fetch_params(0, 2, 0, &key, &value);

	dao_update_property_array(getThis(), SL("_viewParams"), key, value);

}

/**
 * Magic method to retrieve a variable passed to the view
 *
 *<code>
 *	echo $this->view->products;
 *</code>
 *
 * @param string $key
 * @return mixed
 */
PHP_METHOD(Dao_Mvc_View_Simple, __get){

	zval *key, params = {};

	dao_fetch_params(0, 1, 0, &key);

	dao_read_property(&params, getThis(), SL("_viewParams"), PH_NOISY|PH_READONLY);
	if (!dao_array_isset_fetch(return_value, &params, key, PH_COPY)) {
		RETURN_NULL();
	}
}