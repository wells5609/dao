
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

#include "mvc/model/metadata/memory.h"
#include "mvc/model/metadata.h"
#include "mvc/model/metadatainterface.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"

/**
 * Dao\Mvc\Model\MetaData\Memory
 *
 * Stores model meta-data in memory. Data will be erased when the request finishes
 *
 */
zend_class_entry *dao_mvc_model_metadata_memory_ce;

PHP_METHOD(Dao_Mvc_Model_MetaData_Memory, __construct);
PHP_METHOD(Dao_Mvc_Model_MetaData_Memory, read);
PHP_METHOD(Dao_Mvc_Model_MetaData_Memory, write);

ZEND_BEGIN_ARG_INFO_EX(arginfo_dao_mvc_model_metadata_memory___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

static const zend_function_entry dao_mvc_model_metadata_memory_method_entry[] = {
	PHP_ME(Dao_Mvc_Model_MetaData_Memory, __construct, arginfo_dao_mvc_model_metadata_memory___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Dao_Mvc_Model_MetaData_Memory, read, arginfo_dao_mvc_model_metadatainterface_read, ZEND_ACC_PUBLIC)
	PHP_ME(Dao_Mvc_Model_MetaData_Memory, write, arginfo_dao_mvc_model_metadatainterface_write, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Dao\Mvc\Model\MetaData\Memory initializer
 */
DAO_INIT_CLASS(Dao_Mvc_Model_MetaData_Memory){

	DAO_REGISTER_CLASS_EX(Dao\\Mvc\\Model\\MetaData, Memory, mvc_model_metadata_memory, dao_mvc_model_metadata_ce, dao_mvc_model_metadata_memory_method_entry, 0);

	zend_class_implements(dao_mvc_model_metadata_memory_ce, 1, dao_mvc_model_metadatainterface_ce);

	return SUCCESS;
}

/**
 * Dao\Mvc\Model\MetaData\Memory constructor
 *
 * @param array $options
 */
PHP_METHOD(Dao_Mvc_Model_MetaData_Memory, __construct){

	zval *options = NULL;

	dao_fetch_params(0, 0, 1, &options);
	
	if (!options) {
		options = &DAO_GLOBAL(z_null);
	}
	
	dao_update_property_empty_array(getThis(), SL("_metaData"));
}

/**
 * Reads the meta-data from temporal memory
 *
 * @param string $key
 * @return array
 */
PHP_METHOD(Dao_Mvc_Model_MetaData_Memory, read){

	zval *key;

	dao_fetch_params(0, 1, 0, &key);
	
	RETURN_NULL();
}

/**
 * Writes the meta-data to temporal memory
 *
 * @param string $key
 * @param array $metaData
 */
PHP_METHOD(Dao_Mvc_Model_MetaData_Memory, write){

	zval *key, *meta_data;

	dao_fetch_params(0, 2, 0, &key, &meta_data);
	
	RETURN_NULL();
}
