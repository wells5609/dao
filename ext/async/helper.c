/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Martin Schröder <m.schroeder2007@gmail.com>                 |
  +----------------------------------------------------------------------+
*/

#include "async/core.h"
#include "async/async_helper.h"

#include "zend_smart_str.h"

char *async_status_label(zend_uchar status)
{
	if (status == ASYNC_OP_RESOLVED) {
		return "RESOLVED";
	}

	if (status == ASYNC_OP_FAILED) {
		return "FAILED";
	}

	return "PENDING";
}

zval *async_prop_write_handler_readonly(zend_object *object, zend_string *member, zval *value, void **cache_slot)
{
	zend_throw_error(NULL, "Cannot write to property \"%s\" of %s", ZSTR_VAL(member), ZSTR_VAL(object->ce->name));
	return NULL;
}

ASYNC_API void async_prepare_throwable(zval *error, zend_execute_data *exec, zend_class_entry *ce, const char *message, ...)
{
	zend_execute_data *current, *prev;

	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	smart_str str = {0};

	va_list argv;

	zend_object *p1;
	zend_object *p2;

	zval arg;
	zval retval;

	current = EG(current_execute_data);
	p1 = EG(exception);
	p2 = EG(prev_exception);

	EG(current_execute_data) = exec;
	EG(exception) = NULL;
	EG(prev_exception) = NULL;

	// The zend_fetch_debug_backtrace will causing memory not to be released
	if (exec) {
		prev = exec->prev_execute_data;
		exec->prev_execute_data = NULL;
	}

	object_init_ex(error, ce);

	va_start(argv, message);

	if (!ce->constructor) {
		va_end(argv);

		EG(current_execute_data) = current;
		EG(exception) = p1;
		EG(prev_exception) = p2;

		if (exec) {
			exec->prev_execute_data = prev;
		}
		return;
	}

	fci = empty_fcall_info;
	fcc = empty_fcall_info_cache;

	ZVAL_STR(&fci.function_name, ce->constructor->common.function_name);

	php_printf_to_smart_str(&str, message, argv);
	ZVAL_STR(&arg, smart_str_extract(&str));

	va_end(argv);

	zend_fcall_info_argp(&fci, 1, &arg);
	zval_ptr_dtor(&arg);

	fci.param_count = 1;
	fci.retval = &retval;
	fci.object = Z_OBJ_P(error);

	fci.size = sizeof(fci);

	fcc.function_handler = ce->constructor;
	fcc.called_scope = ce;
	fcc.object = Z_OBJ_P(error);

	ZVAL_UNDEF(&retval);

	zend_try {
		zend_call_function(&fci, &fcc);
	} zend_catch {
		async_task_scheduler_handle_exit(async_task_scheduler_get());
	} zend_end_try();

	zval_ptr_dtor(&retval);

	zend_fcall_info_args_clear(&fci, 1);
	zval_ptr_dtor(&fci.function_name);

	EG(current_execute_data) = current;
	EG(exception) = p1;
	EG(prev_exception) = p2;

	if (exec) {
		exec->prev_execute_data = prev;
	}
	trace_error_info(error, NULL);
}

int async_get_poll_fd(zval *val, php_socket_t *sock, zend_string **error)
{
	php_socket_t fd;
	php_stream *stream;

	stream = (php_stream *) zend_fetch_resource_ex(val, NULL, php_file_le_stream());
	*error = NULL;

#if ASYNC_SOCKETS
	php_socket *socket;
	if (!stream && (socket = Z_SOCKET_P(val))) {
#ifdef PHP_WIN32
		if (UNEXPECTED(socket->bsd_socket == INVALID_SOCKET)) {
#else
		if (UNEXPECTED(socket->bsd_socket < 1)) {
#endif
			*error = zend_string_init(ZEND_STRL("Underlying file descriptor is invalid (or closed)"), 0);
			return FAILURE;
		}

		*sock = socket->bsd_socket;

		return SUCCESS;
	}
#endif

	if (UNEXPECTED(!stream)) {
		*error = zend_string_init(ZEND_STRL("Unable to fetch stream resource"), 0);
		return FAILURE;
	}

	if (stream->wrapper) {
		if (!strcmp((char *)stream->wrapper->wops->label, "PHP")) {
			if (!stream->orig_path || (strncmp(stream->orig_path, "php://std", sizeof("php://std") - 1) && strncmp(stream->orig_path, "php://fd", sizeof("php://fd") - 1))) {
				ASYNC_STRF(*error, "Unsupported stream type: %s", stream->wrapper->wops->label);
				return FAILURE;
			}
		}
	}

	if (UNEXPECTED(php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void *) &fd, 1) != SUCCESS)) {
		*error = zend_string_init(ZEND_STRL("Stream could not be cast to fd"), 0);
		return FAILURE;
	}

#ifdef PHP_WIN32
	if (UNEXPECTED(fd == INVALID_SOCKET)) {
#else
	if (UNEXPECTED(fd < 1)) {
#endif
		*error = zend_string_init(ZEND_STRL("Underlying file descriptor is invalid (or closed)"), 0);
		return FAILURE;
	}

	if (stream->wrapper && !strcmp((char *) stream->wrapper->wops->label, "plainfile")) {
#ifndef PHP_WIN32
		struct stat stat;
		fstat(fd, &stat);

		if (!S_ISFIFO(stat.st_mode)) {
			*error = zend_string_init(ZEND_STRL("Invalid file descriptor"), 0);
			return FAILURE;
		}
#else
		*error = zend_string_init(ZEND_STRL("Invalid file descriptor"), 0);
		return FAILURE;
#endif
	}

	*sock = fd;

	return SUCCESS;
}

void async_helper_init()
{
#if ASYNC_SOCKETS


#endif
}
