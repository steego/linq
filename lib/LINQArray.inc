<?php

namespace Steego\Linq;

/**
 * LINQArray is an object used to fluently transform arrays.  While you can use the
 * constructor, it's recommended you use LINQ::Arr($array) to create an instance.
 **/
class LINQArray {
	/**
	 * Creates an instance of LINQArray
	 * @param (array|LINQArray) $array The array/query/object you want to transform
	 **/
	function __construct($array) {
		if($array instanceof LINQArray) {
			$array = $array->toArray();
		}
		if(is_object($array)) {
			$array = (array) $array;
		}
		$this->array = (is_array($array)) ? $array : array();
	}

	/**
	 * Transforms an array by calling a function over each element
	 * @param callable | string $fn The function which is applied to each element in the array
	 * @return LINQArray A new transformed array
	 */
	public function map($fn) {
		return LINQ::Arr(array_map(LINQ::GetFunction($fn), $this->array));
	}

	/**
	 * Filters elements in the array by calling $fn and keeping items which return true.
	 * @param callable | string $fn The function to test whether an item can stay
	 * @return LINQArray A new filtered array
	 */
	public function filter($fn) {
		return LINQ::Arr(array_filter($this->array, LINQ::GetFunction($fn)));
	}

	/**
	 * Filters elements in the array by calling $fn and removing items which return true
	 * @param callable | string $fn The function to test whether and item should be removed
	 * @return LINQArray A new filtered array
	 */
	public function remove($fn) {
		$fn = LINQ::GetFunction($fn);
		$fComp = function($x) use($fn){return !$fn($x);};
		return LINQ::Arr(array_filter($this->array, $fComp));
	}

	/**
	 * This is a wrapper for PHP's array_reduce function which given an
	 * initial value and a combining function, will return a single result
	 * @param mixed $initial The seed or initial value
	 * @param (callable|string) $f A function which accepts two parameters
	 * @return mixed A single value
	 */
	public function fold($initial, $f) {
		return array_reduce($this->array, $f, $initial);
	}

	/**
	 * Sorts an array in ascending order based on a single property or function
	 * @param callable | string $getSortCriteria A function which returns a value to sort on
	 * @return LINQArray
	 */
	public function sortBy($getSortCriteria) {
		$getSortCriteria = LINQ::GetFunction($getSortCriteria);
		$array = $this->array;
		uasort($array, function($a, $b) use($getSortCriteria) {
			$v1 = $getSortCriteria($a);
			$v2 = $getSortCriteria($b);
			if($v1 == $v2) return 0;
			return ($v1 < $v2) ? -1 : 1;
		});
		return LINQ::Arr($array);
	}

	/**
	 * Sorts an array in descending order based on a single property or function
	 * @param callable | string $getSortCriteria A function which returns a value to sort on
	 * @return LINQArray
	 */
	public function sortByDesc($getSortCriteria) {
		$getSortCriteria = LINQ::GetFunction($getSortCriteria);
		$array = $this->array;
		uasort($array, function($a, $b) use($getSortCriteria) {
			$v1 = $getSortCriteria($a);
			$v2 = $getSortCriteria($b);
			if($v1 == $v2) return 0;
			return ($v1 < $v2) ? 1 : -1;
		});
		return LINQ::Arr($array);
	}

	/**
	 * Similar to fold, but returns an array of the same size with an accumulated
	 * result set.  Used for scenarios like running totals
	 * @param mixed $initial The seed or initial value
	 * @param callable | string $f A function which accepts two parameters and combines it into one
	 * @return LINQArray
	 */
	public function scan($initial, $f) {
		$out = array();
		$result = $initial;
		foreach($this->array as $value) {
			//$result = $value;
			$result = $f($result, $value);
			array_push($out, $result);
		}
		//return LINQ::Arr($out);
		return LINQ::Arr($out);
	}

	/**
	 * Transforms the array to only return unique values
	 * @return LINQArray
	 */
	public function unique() {
		return LINQ::Arr(array_unique($this->array));
	}

	/**
	 * Converts an array into a key/value array given two functions which extract
	 * keys and values from the source array
	 * @param (callable | string) $getKey A function that returns the key value
	 * @param null $getValue A function that returns what will be added to the value part.
	 * @return LINQArray
	 */
	public function toLookup($getKey, $getValue = NULL) {
		return LINQ::Arr($this->toHash($getKey, $getValue));
	}
	
	/**
	 * Exchanges all keys and values, i.e., transposes a PHP array
	 * @return LINQArray
	 */
	public function flip() {
		return LINQ::Arr(array_flip($this->array));
	}

	/**
	 * Converts an array into a key/value array given two functions which extract
	 * keys and values from the source array.  Lastly, we convert it into an object
	 * This is the same as calling $ex1->toHash and converting it to an object
	 * @param (callable | string) $getKey A function that returns the key value
	 * @param null $getValue A function that returns what will be added to the value part.
	 * @return object
	 */
	public function toObject($getKey, $getValue = NULL) {
		return (object) $this->toHash($getKey, $getValue);
	}

	/**
	 * Almost like toHash, this method converts an array into a key/value array.  Unlike
	 * toHash, it makes sure we only have one value for each key by replacing existing keys
	 * with new values
	 * @param $getKey
	 * @param null $getValue
	 * @return array
	 */
	public function toSingleHash($getKey, $getValue = NULL) {
		$getKey = LINQ::GetFunction($getKey);
		if(is_null($getValue)) {
			$getValue = (function($x) { return $x; });
		}
		$getValue = LINQ::GetFunction($getValue);
		$arr = $this->array;
		$output = array();
		foreach($arr as $item){
			$key = $getKey($item);
			$output[$key]= $getValue($item);
		}
		return $output;
	}

	/**
	 * Converts an array into a key/value array given two functions which extract
	 * keys and values from the source array
	 * @param (callable | string) $getKey A function that returns the key value
	 * @param null $getValue A function that returns what will be added to the value part.
	 * @return array
	 */
	public function toHash($getKey, $getValue = null) {
		$getKey = LINQ::GetFunction($getKey);
		if(is_null($getValue)) {
			$getValue = (function($x) { return $x; });
		}
		$getValue = LINQ::GetFunction($getValue);
		$arr = $this->array;
		$output = array();
		foreach($arr as $item){
			$key = $getKey($item);
			$output[$key][]= $getValue($item);
		}
		return $output;
	}

	public function groupBy($fn, $r = NULL) {
		return $this->toLookup($fn, $r)->mapKey(function($value, $key) {
			return (object) compact('key', 'value');
		});
	}

	public function toDeepLookup($getKey, $getValue = NULL) {
		$getKey = LINQ::GetFunction($getKey);
		if(is_null($getValue)) {
			$getValue = (function($x) { return $x; });
		}
		$getValue = LINQ::GetFunction($getValue);
		$result = array();
		$current = &$result;
		foreach($this->array as $item) {
			$keyList = $getKey($item);
			if(!is_array($keyList)) $keyList = array($keyList);

			$current = &$result;
			foreach($keyList as $key) {
				if(!array_key_exists($key, $current)) $current[$key] = array();
				$current = &$current[$key];
			}
			$current[] = $getValue($item);
		}
		return $result;
	}
	
	/**
	 * Similar to map, this transforms a key => value array by calling a function with both keys and values.
	 * It does not provide a way to map to new keys
	 * @param callable | string $f A function which accepts two values and returns a single value
	 * @return LINQArray
	 */
	public function mapKey($f) {
		if(is_null($f)) {
			$f = (function($value, $key) { return array('key' => $key, 'value' => $value); });
		}
		$output = array();
		foreach($this->array as $key => $value){
			$output[]= $f($value, $key);
		}
		return LINQ::Arr($output);
	}

	/**
	 * Calls a function for each item in the array
	 * @param (callable | string) $fn The function to be called.
	 * @return LINQArray Itself
	 */
	public function sendTo($fn) {
		//if(!is_callable($fn)) { throw new Exception("LINQ::sentTo needs a callable object"); }
		return LINQ::Arr(call_user_func($fn, $this->array));
	}

	/**
	 * Returns the arrays values, exlcuding keys
	 * @return LINQArray
	 */
	public function values() {
		return LINQ::Arr(array_values($this->array));
	}

	/**
	 * Flattens a nested array structure
	 * @param callable $f
	 * @param callable $combine
	 * @return LINQArray
	 */
	public function flatten($f = NULL, $combine = NULL) {
		$f = LINQ::GetFunction($f);

		if(is_null($f)) {
			$f = function($record) {
			  return $record;
			};
		}

		if(is_null($combine)) {
			/** @noinspection PhpUnusedParameterInspection */
			$combine = (function($parent, $item) {
				return $item;
			});
		}
		$result = array();
		foreach($this->array as $record) {
			$subRecords = $f($record);
			foreach($subRecords as $subRecord) {
				$result[] = $combine($record, $subRecord);
			}
		}
		return LINQ::Arr($result);
	}

	/**
	 * Converts our LINQArray into a regular PHP array
	 * @return array
	 */
	public function toArray() {
		return $this->array;
	}

	/**
	 * Returns the first element or NULL if the LINQArray is empty
	 * @return mixed | null
	 */
	public function first() {
		if(!$this->array || count($this->array) == 0) return null;
		$sliced = array_slice($this->array, 0, 1);
		if($sliced && is_array($sliced)) return $sliced[0];
		return NULL;
	}
	
	public function last() {
		if(!$this->array || count($this->array) == 0) return NULL;
		return end($this->array);
	}
	
	public function take($n) {
		$result = array();
		$count = 0;
		foreach($this->array as $item) {
			if($count >= $n) { return LINQ::Arr($result); }
			$result[] = $item;
			$count++;
		}
		return LINQ::Arr($result);
	}
	
	public function reverse() {
		$reversed = array_reverse($this->array);
		return LINQ::Arr($reversed);
	}
	
	public function takeWhile($test) {
		$result = array();
		foreach($this->array as $item) {
			if(!$test($item)) {
				return LINQ::Arr($result);
			}
			$result[] = $item;
		}
		return LINQ::Arr($result);
	}	
	
	public function skipWhile($test) {
		$result = array();
		$isSkipping = true;
		foreach($this->array as $item) {
			if($isSkipping == true && !$test($item)) {
				$isSkipping = false;
			}
			if($isSkipping == false) {
				$result[] = $item;
			}
		}
		return LINQ::Arr($result);
	}

	public function streamTo(LINQStreamBase $stream) {
		foreach($this->array as $item) {
			$ret = $stream($item);
			if($ret === false) return false;
		}
		return true;
	}
}


?>
