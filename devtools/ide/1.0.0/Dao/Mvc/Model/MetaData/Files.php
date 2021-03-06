<?php 

namespace Dao\Mvc\Model\MetaData {

	/**
	 * Dao\Mvc\Model\MetaData\Files
	 *
	 * Stores model meta-data in PHP files.
	 *
	 *<code>
	 * $metaData = new \Dao\Mvc\Model\Metadata\Files(array(
	 *    'metaDataDir' => 'app/cache/metadata/'
	 * ));
	 *</code>
	 */
	
	class Files extends \Dao\Mvc\Model\MetaData implements \Dao\Mvc\Model\MetaDataInterface, \Dao\Di\InjectionAwareInterface, \Dao\Events\EventsAwareInterface {

		const MODELS_ATTRIBUTES = 0;

		const MODELS_PRIMARY_KEY = 1;

		const MODELS_NON_PRIMARY_KEY = 2;

		const MODELS_NOT_NULL = 3;

		const MODELS_DATA_TYPES = 4;

		const MODELS_DATA_TYPES_NUMERIC = 5;

		const MODELS_DATE_AT = 6;

		const MODELS_DATE_IN = 7;

		const MODELS_IDENTITY_COLUMN = 8;

		const MODELS_DATA_TYPES_BIND = 9;

		const MODELS_AUTOMATIC_DEFAULT_INSERT = 10;

		const MODELS_AUTOMATIC_DEFAULT_UPDATE = 11;

		const MODELS_DATA_DEFAULT_VALUE = 12;

		const MODELS_DATA_SZIE = 13;

		const MODELS_DATA_SCALE = 14;

		const MODELS_DATA_BYTE = 15;

		const MODELS_COLUMN_MAP = 0;

		const MODELS_REVERSE_COLUMN_MAP = 1;

		protected $_metaDataDir;

		/**
		 * \Dao\Mvc\Model\MetaData\Files constructor
		 *
		 * @param array $options
		 */
		public function __construct($options=null){ }


		/**
		 * Reads meta-data from files
		 *
		 * @param string $key
		 * @return array
		 */
		public function read($key){ }


		/**
		 * Writes the meta-data to files
		 *
		 * @param string $key
		 * @param array $data
		 */
		public function write($key, $data){ }


		public function reset(){ }

	}
}
