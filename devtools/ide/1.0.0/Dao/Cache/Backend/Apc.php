<?php 

namespace Dao\Cache\Backend {

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
	
	class Apc extends \Dao\Cache\Backend implements \Dao\Cache\BackendInterface, \Dao\Di\InjectionAwareInterface, \Dao\Events\EventsAwareInterface {

		/**
		 * Returns a cached content
		 *
		 * @param string $keyName
		 * @return mixed
		 */
		public function get($keyName, $lifetime=null){ }


		/**
		 * Stores cached content into the APC backend and stops the frontend
		 *
		 * @param string $keyName
		 * @param string $content
		 * @param long $lifetime
		 * @param boolean $stopBuffer
		 */
		public function save($keyName=null, $value=null, $lifetime=null, $stopBuffer=null){ }


		/**
		 * Deletes a value from the cache by its key
		 *
		 * @param string $keyName
		 * @return boolean
		 */
		public function delete($keyName){ }


		/**
		 * Query the existing cached keys
		 *
		 * @param string $prefix
		 * @return array
		 */
		public function queryKeys($prefix=null){ }


		/**
		 * Checks if cache exists and it hasn't expired
		 *
		 * @param string $keyName
		 * @return boolean
		 */
		public function exists($keyName){ }


		/**
		 * Increment of a given key, by number $value
		 *
		 * @param string $keyName
		 * @param long $value
		 * @return mixed
		 */
		public function increment($keyName, $value=null){ }


		/**
		 * Decrement of a given key, by number $value
		 *
		 * @param string $keyName
		 * @param long $value
		 * @return mixed
		 */
		public function decrement($keyName, $value=null){ }


		/**
		 * Immediately invalidates all existing items.
		 *
		 * @return boolean
		 */
		public function flush(){ }

	}
}
