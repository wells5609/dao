
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

#include "events/listener.h"
#include "events/exception.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/exception.h"

/**
 * Dao\Events\Listener
 *
 * This class offers contextual information of a fired event in the EventsManager
 */
zend_class_entry *dao_events_listener_ce;

PHP_METHOD(Dao_Events_Listener, __construct);
PHP_METHOD(Dao_Events_Listener, setListener);
PHP_METHOD(Dao_Events_Listener, getListener);
PHP_METHOD(Dao_Events_Listener, setPriority);
PHP_METHOD(Dao_Events_Listener, getPriority);
PHP_METHOD(Dao_Events_Listener, setEvent);
PHP_METHOD(Dao_Events_Listener, getEvent);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_listener___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, listener)
	ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_listener_setlistener, 0, 0, 1)
	ZEND_ARG_INFO(0, listener)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_listener_setpriority, 0, 0, 1)
	ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_events_listener_setevent, 0, 0, 1)
	ZEND_ARG_INFO(0, event)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_events_listener_method_entry[] = {
	PHP_ME(Dao_Events_Listener, __construct, arginfo_dao_events_listener___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Events_Listener, setListener, arginfo_dao_events_listener_setlistener, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Events_Listener, getListener, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Events_Listener, setPriority, arginfo_dao_events_listener_setpriority, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Events_Listener, getPriority, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Events_Listener, setEvent, arginfo_dao_events_listener_setevent, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Events_Listener, getEvent, arginfo_empty, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Events\Listener initializer
 */
DAO_INIT_CLASS(Dao_Events_Listener){

	DAO_REGISTER_CLASS(Dao\\Events, Listener, events_listener, dao_events_listener_method_entry, 0);

	zend_declare_property_null(dao_events_listener_ce, SL("_listener"), ZEND_ACC_PROTECTED);
	zend_declare_property_long(dao_events_listener_ce, SL("_priority"), 100, ZEND_ACC_PROTECTED);
	zend_declare_property_null(dao_events_listener_ce, SL("_event"), ZEND_ACC_PROTECTED);

	return SUCCESS;
}

/**
 * Dao\Events\Listener constructor
 *
 * @param callable $listener
 * @param mixed $priority
 */
PHP_METHOD(Dao_Events_Listener, __construct){

	zval *listener = NULL, *priority = NULL, *event = NULL;

	dao_fetch_params(0, 0, 3, &listener, &priority, &event);

	if (listener) {
		dao_update_property(getThis(), SL("_listener"), listener);
	}

	if (priority) {
		dao_update_property(getThis(), SL("_priority"), priority);
	}

	if (event) {
		dao_update_property(getThis(), SL("_event"), event);
	}
}

PHP_METHOD(Dao_Events_Listener, setListener){

	zval *listener;

	dao_fetch_params(0, 1, 0, &listener);

	dao_update_property(getThis(), SL("_listener"), listener);

}

PHP_METHOD(Dao_Events_Listener, getListener){


	RETURN_MEMBER(getThis(), "_listener");
}

PHP_METHOD(Dao_Events_Listener, setPriority){

	zval *priority;

	dao_fetch_params(0, 1, 0, &priority);

	dao_update_property(getThis(), SL("_priority"), priority);

}

PHP_METHOD(Dao_Events_Listener, getPriority){


	RETURN_MEMBER(getThis(), "_priority");
}

PHP_METHOD(Dao_Events_Listener, setEvent){

	zval *event;

	dao_fetch_params(0, 1, 0, &event);

	dao_update_property(getThis(), SL("_event"), event);

}

PHP_METHOD(Dao_Events_Listener, getEvent){


	RETURN_MEMBER(getThis(), "_event");
}
