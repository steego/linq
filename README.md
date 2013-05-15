# Steego Linq

Steego Linq is yet another PHP implementation based on Microsoft's LINQ.  Unlike LINQ true ports, like RxJS, Steego Linq
does not attempt to truly capture the semantics of the Ix and Rx libraries.

# Usage

```php
<?php

use Steego\Linq\LINQ;

//  Let's double everything
$doubledNumbers = LINQ::Arr([1, 2, 3, 4])->map(function($x) { return $x * $x; })->toArray();

//  Let's keep even numbers
$evenNumbers = LINQ::Arr([1, 2, 3, 4])->filter(function($x) { return $x % 2 == 0; })->toArray();
```

# Get It

Steego Linq is an unofficial PHP Composer package.

Add the following sections to your composer.json by adding the following sections:

```json
  "require": {
		"Steego/Linq": "master"
	}
```

You'll need to explicitly tell it where the repository is:

```json
  "repositories": [
		{
			"type": "package",
			"package": {
				"name": "Steego/Linq",
				"version":"master",
				"source": {
					"url": "git@github.com:steego/linq.git",
					"type": "git",
					"reference": "master"
				}
			}
		}
	]
```




# License

Steego Linq is licensed under the MIT License
