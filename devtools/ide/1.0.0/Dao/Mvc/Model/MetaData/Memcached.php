<?php 

namespace Dao\Mvc\Model\MetaData {

	/**
	 * Dao\Mvc\Model\MetaData\Memcached
	 *
	 * Stores model meta-data in the Memcached cache. Data will erased if the web server is restarted
	 *
	 * By default meta-data is stored for 48 hours (172800 seconds)
	 *
	 * You can query the meta-data by printing memcached_get('$PMM$') or memcached_get('$PMM$my-app-id')
	 *
	 *<code>
	 *	$metaData = new Dao\Mvc\Model\Metadata\Memcached(array(
	 *     'servers' => array(
	 *         array('host' => 'localhost', 'port' => 11211, 'weight' => 1),
	 *     ),
	 *     'client' => array(
	 *         Memcached::OPT_HASH => Memcached::HASH_MD5,
	 *         Memcached::OPT_PREFIX_KEY => 'prefix.',
	 *     ),
	 *		'prefix' => 'my-app-id',
	 *		'lifetime' => 86400
	 *	));
	 *</code>
	 */
	
	class Memcached extends \Dao\Mvc\Model\MetaData implements \Dao\Mvc\Model\MetaDataInterface, \Dao\Di\InjectionAwareInterface, \Dao\Events\EventsAwareInterface {

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

		protected $_lifetime;

		protected $_memcached;

		/**
		 * \Dao\Mvc\Model\MetaData\Memcached constructor
		 *
		 * @param array $options
		 */
		public function __construct($options=null){ }


		/**
		 * Reads metadata from Memcached
		 *
		 * @param  string $key
		 * @return array
		 */
		public function read($key){ }


		/**
		 *  Writes the metadata to Memcached
		 *
		 * @param string $key
		 * @param array $data
		 */
		public function write($key, $data){ }


		public function reset(){ }

	}
}
