<?php 

namespace Dao\Mvc\Model\Resultset {

	/**
	 * Dao\Mvc\Model\Resultset\Complex
	 *
	 * Complex resultsets may include complete objects and scalar values.
	 * This class builds every complex row as it is required
	 */
	
	class Complex extends \Dao\Mvc\Model\Resultset implements \JsonSerializable, \Serializable, \ArrayAccess, \Countable, \SeekableIterator, \Traversable, \Iterator, \Dao\Mvc\Model\ResultsetInterface, \Dao\Di\InjectionAwareInterface, \Dao\Events\EventsAwareInterface {

		const TYPE_RESULT_FULL = 0;

		const TYPE_RESULT_PARTIAL = 1;

		const HYDRATE_RECORDS = 0;

		const HYDRATE_OBJECTS = 2;

		const HYDRATE_ARRAYS = 1;

		protected $_sourceModel;

		protected $_columnTypes;

		protected $_rowsModels;

		protected $_rowsObjects;

		protected $_rowsArrays;

		/**
		 * \Dao\Mvc\Model\Resultset\Complex constructor
		 *
		 * @param \Dao\Mvc\ModelInterface $sourceModel
		 * @param array $columnsTypes
		 * @param \Dao\Db\ResultInterface $result
		 * @param \Dao\Cache\BackendInterface $cache
		 */
		public function __construct($columnsTypes, $result, $cache=null, $sourceModel=null){ }


		/**
		 * Check whether internal resource has rows to fetch
		 *
		 * @return boolean
		 */
		public function valid(){ }


		/**
		 * Returns a complete resultset as an array, if the resultset has a big number of rows
		 * it could consume more memory than currently it does.
		 *
		 * @return array
		 */
		public function toArray(){ }


		/**
		 * Serializing a resultset will dump all related rows into a big array
		 *
		 * @return string
		 */
		public function serialize(){ }


		/**
		 * Unserializing a resultset will allow to only works on the rows present in the saved state
		 *
		 * @param string $data
		 */
		public function unserialize($serialized=null){ }

	}
}
