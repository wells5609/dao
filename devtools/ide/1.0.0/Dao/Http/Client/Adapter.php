<?php 

namespace Dao\Http\Client {

	/**
	 * Dao\Http\Client\Adapter
	 */
	
	abstract class Adapter implements \Dao\Http\Client\AdapterInterface {

		const VERSION = 0.0.1;

		const AUTH_TYPE_ANY = any;

		const AUTH_TYPE_BASIC = basic;

		const AUTH_TYPE_DIGEST = digest;

		protected $_header;

		protected $_base_uri;

		protected $_method;

		protected $_useragent;

		protected $_username;

		protected $_password;

		protected $_authtype;

		protected $_digest;

		protected $_entity_body;

		protected $_data;

		protected $_type;

		protected $_files;

		protected $_timeout;

		/**
		 * Sets the value of the userAgent property
		 *
		 * @param string $useragent
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setUserAgent($useragent){ }


		/**
		 * Set authentication credential
		 *
		 * @param string $username
		 * @param string $password
		 * @param string $authtype
		 * @param array $digest
		 * @param string $entityBody
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setAuth($username, $password, $authtype=null, $digest=null, $entityBody=null){ }


		/**
		 * Set header
		 *
		 * @param string $name
		 * @param string $value
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setHeader($name, $value){ }


		/**
		 * Set headers
		 *
		 * @param array $headers
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setHeaders($headers){ }


		/**
		 * Set data
		 *
		 * @param array|string $data
		 * @param string $type example: application/json
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setData($data, $type=null){ }


		/**
		 * Set send file
		 *
		 * @param string $file
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setFile(){ }


		/**
		 * Set send files
		 *
		 * @param array|string $files
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setFiles($files){ }


		/**
		 * Retrieve the URI path
		 *
		 * @return string
		 */
		public function getPath(){ }


		/**
		 * Send GET request
		 *
		 * @param string $uri
		 * @param string $data
		 * @return \Dao\Http\Client\Response
		 */
		public function get($uri=null, $data=null, $type=null){ }


		/**
		 * Send HEAD request
		 *
		 * @param string $uri
		 * @param string $data
		 * @return \Dao\Http\Client\Response
		 */
		public function head($uri=null, $data=null, $type=null){ }


		/**
		 * Send POST request
		 *
		 * @param string $uri
		 * @param string $data
		 * @return \Dao\Http\Client\Response
		 */
		public function post($uri=null, $data=null, $type=null){ }


		/**
		 * Send PUT request
		 *
		 * @param string $uri
		 * @param string $data
		 * @return \Dao\Http\Client\Response
		 */
		public function put($uri=null, $data=null, $type=null){ }


		/**
		 * Send DELETE request
		 *
		 * @param string $uri
		 * @param string $data
		 * @return \Dao\Http\Client\Response
		 */
		public function delete($uri=null, $data=null, $type=null){ }


		/**
		 * Set URI
		 *
		 * @param string $uri
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setUri($uri){ }


		/**
		 * Get URI
		 *
		 * @return \Dao\Http\Uri
		 */
		public function getUri(){ }


		/**
		 * Set base URI
		 *
		 * @param string $uri
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setBaseUri($uri=null){ }


		/**
		 * Set method
		 *
		 * @param string $uri
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setMethod($method){ }


		/**
		 * Set the request timeout
		 *
		 * @param string $uri
		 * @return \Dao\Http\Client\Adapter
		 */
		public function setTimeOut($time){ }


		/**
		 * Send request
		 *
		 * @return \Dao\Http\Client\Response
		 */
		public function send($uri=null){ }


		abstract protected function sendInternal();

	}
}
