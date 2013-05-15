<?php


namespace Steego\Linq;

class LINQStreamBase {
	private $subscribers = array();
	public function __construct() {
	}
	public function __invoke($value) {
		return $this->call($value);
	}
	protected function call($value) {
		$anyTrue = false;
		foreach($this->subscribers as $key => $sub) {
			$returnValue = $sub($value);
			if($returnValue === false) {
				unset($this->subscribers[$key]);
			} else {
				$anyTrue = true;
			}
		}
		return $anyTrue;
	}

	/**
	 * @param $f
	 *
	 * @return LINQStreamBase
	 */
	public function subscribe($f) {
		$f = LINQ::GetFunction($f);
		$this->subscribers[] = $f;
	}

	public function map($f) {
		$newStream = new LINQStreamMap($f);
		$this->subscribe($newStream);
		return $newStream;
	}
	public function filter($f) {
		$newStream = new LINQStreamFilter($f);
		$this->subscribe($newStream);
		return $newStream;
	}
	public function take($count) {
		$newStream = new LINQStreamTake($count);
		$this->subscribe($newStream);
		return $newStream;
	}
	public function sampleEvery($sampleTime) {
		$newStream = new LINQStreamSampler($sampleTime);
		$this->subscribe($newStream);
		return $newStream;
	}
	public function count() {
		$newStream = new LINQStreamCounter();
		$this->subscribe($newStream);
		return $newStream;
	}
	public function feedTo($fn) {
		return $fn($this);
	}
}

class LINQStream extends LINQStreamBase {
	private $type = "source";
}

class LINQStreamMap extends LINQStreamBase {
	private $mapFunction = null;
	private $type = "map";
	public function __construct($mapFunction) {
		$this->mapFunction = LINQ::GetFunction($mapFunction);
	}
	protected function call($value) {
		$f = $this->mapFunction;
		return parent::call($f($value));
	}
}

class LINQStreamTake extends LINQStreamBase {
	private $count;
	private $type = "take";
	public function __construct($count) {
		$this->count = $count;
	}
	protected function call($value) {
		if($this->count <= 0) return false;
		$ret = parent::call($value);
		$this->count -= 1;
		return $ret;
	}
}

class LINQStreamFilter extends LINQStreamBase {
	private $filterFunction = null;
	private $type = "filter";
	public function __construct($filterFunction) {
		$this->filterFunction = LINQ::GetFunction($filterFunction);
	}
	protected function call($value) {
		$f = $this->filterFunction;
		if(!$f($value)) return true;
		return parent::call($value);
	}
}

class LINQStreamSampler extends LINQStreamBase {
	private $sampleTime = 1;
	private $lastTime = 0;
	private $type = "sampler";
	public function __construct($sampleTime) {
		$this->sampleTime = $sampleTime;
	}
	protected function call($value) {
		$currentTime = time();
		$isSample = ($this->lastTime < $currentTime - $this->sampleTime || $this->lastTime == 0);
		if($isSample) {
			$this->lastTime = $currentTime;
			return parent::call($value);
		}
	}
}

class LINQStreamCounter extends LINQStreamBase {
	private $count = 0;
	private $type = "counter";
	protected function call($value) {
		return parent::call($this->count++);
	}
}