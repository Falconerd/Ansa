# Programming Language for Games

## Goals

- Standard library easy enough for hobbyists
- Language core powerful enough for professionals

## Functions

Functions can be denoted in the same way as other values.

## Style 0:

```c
// param types will carry over to the following parameters
add :: (x f, y) f {
	return x + y;
}

// mutliple return values
split :: (sum i) (i, i) {
	x := sum * 4 / 9;
	y := sum - x;

	return x, y;
}

// swap two strings
swap :: (x s, y) (s, s) => y, x;

// types with properties can have constructors
// type properties can use type inference
type animal (legs u) {
	legs, // will be same type as constructor
	eyes := 2 // will become u8
};

// define an instance of animal
myDog := animal(4);

// put an animal on the stack
myMantis := new animal(2);

type fcallback (x f, y) f;

// even in types with properties, the type carries down (or right)
type point { x f, y };
type point {
	x f,
	y
};

do_something :: (a f, fn (x f, y) f) {
	b := a * fn(4, 3);
}

main :: () {
	a := 4; // will auto to u8
	b: f = 4;

	// will fail because a is u8
	do_something(a, (x f, y f) f {
		return x + y;
	});
	
	// same as above, but one liner
	do_something(b, (x f, y) f => x + y);
	do_something(b, fcallback => x + y);
	
	my_print_wrapper := (str s) => print;
	
	my_print_wrapper("Yay"); // => "Yay"
}

// any function with only 1 param can have use a shorthand _

add_three :: (x f) => x + 3;

results: i[];

array_add(results, 3);
array_add(results, 2);
array_add(results, 1);

for results print("% -> ", _); // => 3 -> 2 -> 1 -> 

// multiple declarations on one line
x, y, z := 42, 23, 57;
```

### Style 1:

```c
my_function := (x: float, y: float): float {
    return x + y;
}

my_function := (x: float, y: float): float => x + y;

do_something := (a: float, fn: (x: float, y: float): float) {
	b := a * fn(3, 4);
}

main := () {
	a: float = 4;
	do_something(a, ((x: float, y: float): float) {
        	return x + y;
	});
}
```
### Style 2:

```c
my_function := (float x, float y) -> float {
	return x + y;
}

my_function := (float x, float y) -> float => x + y;

do_something := (float a, fn(float x, float y) -> float) {
	b := a * fn(3, 4);
}

main := () {
    float a = 4;
    do_something(a, ((float x, float y) {
    	return x + y;
    });
}
```

### Style 3:

```c
float my_function(float x, float y) {
	return x + y;
}

float my_function(float x, float y) => x + y;

do_something(float a, float fn(float x, float y)) {
	b := a * fn(4, 3);
}

main() {
	float a = 4;
	do_something(a, (float x, float y) {
		return x + y;
	});
}
```

### Style 4:

```c
float my_function :: float x float y {
	return x + y;
}

float my_function :: float x float y => x + y;

do_something float a float fn(float x float y) {
	b := a * fn(4, 3);
}

main() {
	float a = 4;
	do_something(a, (float x float y) {
	});
}
```

```c
my_function float x float y {
	return x + y;
}

my_fn float x float y => x + y;

do_something float a float fn(float x float y) {
	b := 
}
```

```typescript
type float = number;
function my_function(x: float, y: float): float {
	return x + y;
}

const my_fn = (x: float, y: float): float => x + y;

function do_something(a: float, fn: (x: float, y: float) => float) {
	let b = a * fn(4, 3);
}

let a: float = 4;
do_something(a, my_fn);
do_something(a, my_fn);
```

```
someVar := SomeFunc(notReally)

Print(someVar);

u32 a = 1234;

u32 SomeFunc(bool arg1) {
	DoSomethingTo(arg1);
	return 8;
}
```

types can have number in them, only the standard types suffix numbers are special

```
type v2 = { f x, f y };
type v3 = { f x, f y, f z };
type v2i = { i x, i y };
type v2u = { u x, u y };
```

No return value  or type required for "void" functions

```
SomeFunc(v3 myVec) {
	DoSomethingWith(myVec);
}
```

## Modules

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



## Literal Types

Any number types can be apended with a number to specify how many bits they
require. Not all numbers have the same available size, so take a look at the
table below. If no number is specified, the smallest will be used.

| Symbol | Type                  | Bit values    |
| ------ | --------------------- | ------------- |
| u      | Unsigned integer      | 8, 16, 32, 64 |
| i      | Signed integer        | 32, 64        |
| f      | Floating point number | 32, 64        |
| s      | String                |               |

eg: `f32` is a 32-bit floating point number

### Why are the types using such short-hand? Isn't that bad for readability?

Most C and C++ programmers begin writing something like this after a while:

```
typedef u8 unsigned uint8_t;
typedef u16 unsigned uint16_t;
typedef u32 unsigned uint32_t;
typedef u64 unsigned uint64_t;
// ... + more
```

Given this is the case, it makes sense to just make that the default way to refer
to the standard types.

As for readability, there is an argument that it's easy to confuse types if they
are just using one character. We tend to process english in terms of words, so when
we see single characters it's harder for us to interpret.

While this is true, this language has type inference which can be used for the standard
types.

So, rather than seeing something like:
```
u someVar = 8;
u someOtherVar = 32;
i thisOneShouldBeInt = -28;
u butSoShouldThisOne = -32;
u someOtherVar = 32;
```

And maybe having a type error on the 4th, instead you would have:

```c
someVar := 8; // u8
someOtherVar := 32; // u8
thisOneShouldBeInt := -28; // i8
butSoShouldThisOne := -32; // i8
someOtherVar := 32; // u8
```

## Records

Records are a special type which can contain other types nested inside them.

```
value := {
	f x;
	f y;
	f z;
	f w;
	u alive = 0;
}
```

The value variable will be of type `{ f32 x, f32 y, f32 z, f32 w, u8 alive }`.

## User-defined types

Users can use the powerful type system to define any types they wish.

When declaring types which utilize the literal types, you must supply the maximum
byte length in the case of number, unless your defined type uses the minimum.

A type with two 32-bit floats, `x` and `y`

```
type vector2 = { f x, f y };
```

If you want to store 64-bit floats, you must specify

```
type vector2 = { f64 x, f64 y };
```

The same rule applies to signed and unsigned integers.

> *A type defined as two unsigned integers `x` and `y` with values 0-255 (inc.)*

```
type myType { u x, u y };

myType a = { 3, 4 };
// => OK

myType b = { 300, 4 };
// => NOT OK, 300 is > 255
```

A type with two unsigned integers with values 0-4294967295 (inc.)
```
type myType { u32 x, u32 y };
```

```
type LivingQuaternion = { f x, f y, f z, f w, u alive = 0 };
```

## Types are structural

This type is compatible with the record from earlier, even though the record was not
declared as a LivingQuaternion.

This is becauese types are structural rather than semantic. This means that if you
have a function which requires a v2 `type v2 = { x: 32, y: f32 };` and you feed it with
a record containing two `f32` values with the names x and y, it should still work.

```
f some_func()

```


## Some standard types for game development are provided in the standard libraries
```
type False = 0
type True = 1
type Bool = False | True
type Vector2 = {
	f32 x;
	f32 y;
}
type Vector3 = {
	f32 x;
	f32 y;
	f32 z;
}

u8 char = "b";
u8 char = 233;
u8 char = "beast";
// Error => Tried to assign data which is too large.

u32 some_value = 12389710317
i8 ... // Error => Signed integers must be either 32 bit or 64 bit.

s what = "the fox jumped over the hen"
```

## Type inference
```
value := 3; // u8
value := -1; // i32
value := "c" // u8 99
// Why? Because a char stored as a number only takes 8 bits.
// A string comes with the length, which in this case is 1, and a waste of memory
value := "ㅕ" // string with a length of 3 (Korean characters are 24 bit)
// Since this value cannot fit into an unsigned int, it is coerced into a string
```

## Strings are buffers with a length, we don't care what is in them
```
value := "God is in his \x48\x65\x61\x76\x65\x6e. All is right with the world."

value.length
// => 50;
```

## Robust type system like Elm

### Union types will have to allocate enough memory for the largest variant
type SomeType = u8 | f32 // Size is 32 bits

Type definitions can be followed by a number to declare a set number of bits

u 8 unsigned 1 byte
u 32 unsigned 4 byte


## Declaring Variables
```
a := 3 // Declares an integer
```

## Chunks?

256 byte chunks (1 cache line)


## Closures

Yes, we need them.

Contrived example.

```c
Book * best_selling_books(Book *books, int len, int threshold) {
	Book* ret = NULL;
	int count = 0;
	for (int i = 0; i < len; ++i) {
		if (books[i].sales >= threshold) {
			ret = realloc(++count * sizeof(Book));
			ret[count-1] = books[i];
		}
	}
	return ret;
}
```

```
// Function version
Book[] best_selling_books(Book[] books, int threshold) {
	return books.filter(_.sales >= threshold);

}

best_selling_books = books.filter(_.sales >= threshold);
```

## Arrays have built-in functionality

```
myArray := [1, 2, 3, 4];

print(myArray.filter(_ < 3)); // => [1, 2]
print(myArray.map(_ + 1)); // => [2, 3, 4, 5]
print(myArray.map((x) => x * 2)); // => [2, 4, 6, 8]
print(myArray.map((item) => {
	// do some side effect if you want
	return item * 4;
})); // => [4, 8, 12, 16]
```

```c

type Entity {
	id: u;
}


e: Entity;

// type decl
ptr: >Entity;
// value decl
ptr = >e;

ptr: >Entity = >e;

ptr := >e;
e2: Entity = <ptr;


my_arr: [30] i;


for my_arr
	print("% %", _i, _v);


```

## SOA by default

```c
type Entity {
	id1 := 1
	id2 := 2
	id3 := 3
}

my_entity : Entity
entities :: [3] Entity

print("% % %\n", my_entity.id1, my_entity.id2, my_entity.id3)
for entities print("% % %\n", ..._v)
	
/*
1, 2, 3
1, 1, 1
2, 2, 2
3, 3, 3
*/
```

## Blank identifier

```

my_fn(_, b);
```
