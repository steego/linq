<?php 

namespace Steego\Linq;

use Exception;

class LINQ {
	public static function Arr($array) {
		return new LINQArray($array);
	}

	public static function Stream() {
		return new LINQStream();
	}

	public static function Root($single) {
		return new LINQArrayTree($single);
	}

	/**
	 * @param $f
	 * @return callable
	 * @throws Exception
	 */
	public static function GetFunction($f) {
		if(is_callable($f)) return $f;
		if(is_string($f)) {
			//  If matches "new <name>" format, it's a constructor
			if(preg_match(':^new [\\\\\w]+:', $f)) {
				$className = preg_replace(':^new :', '', $f);
				return function($param) use($className) {
					return new $className($param);
				};
			}
			//  If matches ".<name>" format, it's a property
			if(preg_match(':^\.[\\\\\w]+:', $f)) {
				$propertyName = preg_replace(':^\.:', '', $f);
				return function($param) use($propertyName) {
					if(is_array($param)) {
						return $param[$propertyName];
					} else if(is_object($param)) {
						return $param->$propertyName;
					}
					return null;
				};
			}
			
			$key = preg_replace(':^\.:', '', $f);
			return (function($input) use($key) {
				if(is_array($input)) {
					return array_key_exists($key, $input) ? $input[$key] : null;
				}
				if(is_object($input)) {
					return $input->$key;
				}
				return null;
			});
		}
		if(is_array($f)) {
			$keys = $f;
			return (function($input) use($keys) {
				$isObject = is_object($input);
				$array = ($isObject) ? (array) $input : $input;
				$output = array();
				foreach($keys as $key) {
					$output[$key] = array_key_exists($key, $array) ? $array[$key] : null;
				}
				return ($isObject) ? (object) $output : $output;
			});
		}
		throw new Exception;
	}
}

//class LINQSeq implements \IteratorAggregate {
//	public function __construct($iterator) {
//		$this->iterator = $iterator;
//	}
//	public function getIterator() {
//		if(is_callable($this->iterator)) {
//			$f = $this->iterator;
//			$result = $f();
//			if(!$result) return null;
//			if(is_array($result)) {
//				return new \ArrayIterator($result);
//			}
//			if($result instanceOf Iterator) { return $result; }
//			return null;
//			//return $result;
//		}
//		return $this->iterator;
//	}
//	public function toArray() {
//		$result = array();
//		foreach($this->getIterator() as $value) {
//			$result[] = $value;
//		}
//		return $result;
//	}
//	public function map($fn) {
//		return new LINQSeq(new MapIterator($this, $fn));
//	}
//}


?>
