# Modules

Modules can be imported partially or entirely by files which require them.

To export a function from a module, it should be prefixed with the `export`
keyword. The same is true for variables of any kind.

> some_func_lib

```
export type number = i32 | u32 | f32;

export int Add(number a, number b) {
	return a + b;
}

export someVar := 42;

export default { number, Add };
```

These can be imported in another module for use.

```
import { number, Add, someVar } from "some_func_lib";

number theMeaningOfLife = Add(0, someVar);

Print("The meaning is: %.", theMeaningOfLife);
// => The meaning is 42.
```

```
import some_func_lib, { someVar } from "some_func_lib";

theMeaningOfLife := some_func_lib.Add(0, someVar);
```

Functions

import { SomeFunc } from "some_func_lib";
// SomeFunc is available, but all other parts of some_func_lib are notimported

import SOME_FUNC_LIB from "some_func_lib";
