<?php 

namespace Dao\Mvc\Model {

	interface BehaviorInterface {

		public function notify($type, $model);


		public function missingMethod($model, $method, $arguments=null);

	}
}
