<?php
/**
 * Created by JetBrains PhpStorm.
 * User: sgoguen
 * Date: 5/15/13
 * Time: 6:31 PM
 * To change this template use File | Settings | File Templates.
 */

namespace Steego\Linq\tests;

use Steego\Linq\LINQArray;

class LINQArrayTest extends \PHPUnit_Framework_TestCase {
	public function testConstructor() {
		$arr1 = new LINQArray([1, 2, 3]);
		$arr2 = new LINQArray($arr1);
		$this->assertEquals($arr1, $arr2);
		$arr3 = new LINQArray([1, 2]);
		$this->assertNotEquals($arr1, $arr3);
	}
	public function testToArray() {
		$arr1 = new LINQArray([1, 2, 3]);
		$this->assertEquals([1,2,3], $arr1->toArray());
	}

	public function testFilter() {
		//  Let's create an array
		$arr1 = new LINQArray([1, 2, 3]);
		//  And keep all the even ones
		$filtered = $arr1->filter(function($x) { return $x % 2 == 0; })->toArray();
		//  We should get one item
		$this->assertEquals(1, count($filtered));
		//  But it's not going to be the same as [2]
		$this->assertNotEquals([2], $filtered);
		//  It will instead keep its position (PHP arrays are not real arrays)
		$this->assertEquals(array(1=>2), $filtered);
	}

	public function testMap() {
		//  Let's create an array
		$arr1 = new LINQArray([1, 2, 3]);
		//  And Square everything
		$filtered = $arr1->map(function($x) { return $x * $x; })->toArray();
		//  We should get squared numbers
		$this->assertEquals([1,4,9], $filtered);
	}

	public function testSortBy() {
		//  Let's create an array
		$arr1 = new LINQArray([3, 2, 1]);
		//  Let's sort it
		$sorted = $arr1->sortBy(function($x) { return $x; })->toArray();
		//  Again, PHP arrays sorted will have bogus keys so let's get rid of them
		$sorted = array_values($sorted);
		//  Now the arrays should match
		$this->assertEquals([1,2,3], $sorted);
	}
}
