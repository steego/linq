<?php

namespace Steego\Linq;;

class LINQArrayTree {
	public function __construct($value) {
		$this->value = $value;
		$this->children = [];
	}
	public function ExpandMax($level, $getChildren) {
		if($level < 1) return $this;

		$expand = function($value) use($level, $getChildren) {
			$tree = new LINQArrayTree($value);
			return $tree->ExpandMax($level - 1, $getChildren);
		};
		if(is_callable($getChildren)) {
			$result = $getChildren($this->value);
			if(is_array($result)) {
				$this->children = array_map($expand, $result);
			}
		}

		return $this;
	}
	private function setChildren($children) {
		$this->children = $children;
		return $this;
	}
	public function map($f) {
		$f = LINQ::GetFunction($f);
		$mapChild = function($n) use($f) {
			return $n->map($f);
		};
		$r = new LINQArrayTree($f($this->value));
		return $r->setChildren(array_map($mapChild, $this->children));
	}
	// flatMap:  (a -> [b]) -> Tree a -> Tree b
	public function mapDown($combine) {
		$mapNode = function($nodeValue) use($combine) {
			return $combine($this->value, $nodeValue);
		};
		$r = new LINQArrayTree($this->value);
		return $r;
	}
	public function filter($f) {
		$f = LINQ::GetFunction($f);
		$keepNode = function($n) use($f) {
			return $f($n->value);
		};
		$filter = function($n) use($f) {
			return $n->filter($f);
		};
		$r = new LINQArrayTree($this->value);
		return $r->setChildren(array_map($filter, array_filter($this->children, $keepNode)));
	}

	/**
	 * Walks the array depth first, calling the user supplied function at each
	 * node
	 * @param $callback
	 */
	private function walkTopDownDepthFirst($callback) {
		$callback($this->value);
		foreach($this->children as $node) {
			$node->walkTopDown($callback);
		}
	}

	/**
	 * Walks the array depth first, calling the user supplied function upon return
	 * @param $callback
	 */
	private function walkBottomUpDepthFirst($callback) {
		foreach($this->children as $node) {
			$node->walkTopDown($callback);
		}
		$callback($this->value);
	}

	private function toArrayUsing($method) {
		$result = [];
		$addResult = function($value) use(&$result) {
			$result[] = $value;
		};
		$this->$method($addResult);
		return $result;
	}

	//  foldTree:  Tree<a> -> (a -> [r] -> r) -> r
	public function foldTree($combine) {
		$value = $this->value;
		$childrenResults = array_map(function($child) use($combine) {
			return $child->foldTree($combine);
		}, $this->children);
		return $combine($value, $childrenResults);
	}

	public function toArray() {
		return $this->toArrayUsing('walkTopDownDepthFirst');
	}
	public function toArrayDepthFirst() {
		return $this->toArrayUsing('walkBottomUpDepthFirst');
	}
	public function toLinqArr() {
		return LINQ::Arr($this->toArray());
	}

}


?>