<?php

namespace Steego\Linq\tests;

use Steego\Linq\LINQ;

class LINQTest extends \PHPUnit_Framework_TestCase {
	public function testConstructor() {
		$arr = LINQ::Arr([1, 2]);
		$this->assertEquals([1, 2], $arr->toArray());
	}
}
