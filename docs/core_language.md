# Core Language

There are three main features you need to be comfortable with to use Ansa:
**Values**, **Pointers**, and **Functions**.

## Values

In Ansa, the most basic building blocks are values. This includes things such
as `23`, `35.5` and `"Hello there!"`.

Let's start by looking at the numeric types.

### Numbers

```
u someNumber = 32
i anotherOne = -1
f andAnother = 35.5
```

The prefixes here are telling the compiler what type of number these are.

- `u` denotes an unsigned integer, which means the lowest number is 0.
- `i` denotes a signed integer, which means the number can be lower than 0.
- `f` denotes a floating point number, these can go below 0 and also contain
up to 7 decimal places.

You can let the compiler figure out (infer) the type by using the special `:=`
syntax.

```
whatAmI := 42
```

In this instance, the compiler will set the type to `u8`. Let's go over what
the 8 means.

There are several suffixes which can be added to the standard number types to
tell the compiler you want a specific size. By default, the compiler will use
the smallest size which can fit the variable.

| Type | Min | Max |
| ---- | --- | --- |
| `u8`   | 0   | 255 |
| `u16`  | 0   | 35 565 |
| `u32`  | 0   | 4 294 967 295 |
| `u64`  | 0   | 2<sup>63</sup>-1 |
| `i8`   | -128 | 127 |
| `i16`  | -32 768 | 32 767 |
| `i32`  | -2 147 483 648 | 2 147 483 647 |
| `i64`  | -2<sup>63</sup> | 2<sup>63</sup>-1 |


Floating point numbers follow the IEEE 754 standard. Due to precision issues,
there are no floating point numbers smaller than 32 bits.

| Type | Min | Max |
| ---- | --- | --- |
| `f32`  | 1.175494351 E-38 | 3.402823466 E+38 |
| `f64`  | 2.2250738585072014 E-308 | 1.7976931348623158 E+308 |

Let us take the following example:

```
howBig := 35000
```

Since most of us are used to typing `int howBig = 35000`, we may expect that
this would be a signed integer type. However, that would be incorrect.

The type of the variable `howBig` would be `u16`.

By default the compiler will use unsigned integers. If it were signed integers,
then this value would have a type of `i32`, which is double the size.

If you need a variable to go below 0, make sure to define the type specifically
as as type starting with `i`.

### Strings

The only non-numeric built in type is the string. Strings are denoted by their
type `s` and must be surrounded with quotation marks.

```
s greetingText = "Hello there!"
```

They can also be inferred by the compiler using the special `:=` syntax.

```
greetingText := "Hello there!"
```

Unlike C and some other languages, there is no `char` or `character` type.

```
someChar := 'a'
// This is NOT valid and will throw a compile error
char someChar = 'a'
// This is NOT valid and will throw a compile error
```

However, if a string with one character that is defined in the ASCII standard
is defined, it will be converted into a `u8` behind the scenes.

This is because the string type contains an internal length counter, which is
a `u64`.
```
someChar := "a"

Type(someChar) // => "u8"
Print("%", someChar) // => a
Print("%d", someChar) // => 61
Size(someChar) // => 8

twoChars := "ab"

Print("%", twoChars) // => ab
Print("%", twoChars.length) // => 2
Type(twoChars.length) // => "u64"
Size(twoChars) // => 80
```

## Pointers

Pointers like values but their type is always `u64` and the data they hold is
the memory address of a different value.

To denote a value as a pointer, the symbol `*` is used as a suffix to the type.

To take the memory address of an existing value, the symbol `&` is used.

```
someNumber := 42
u8* somePointer = &someNumber
```

Pointers can also have their type inferred by the compiler.

```
somePointer := &someNumber
Type(somePointer) // => "u8*"
```

To retrieve or modify the data inside a value, it can be "dereferenced" with
the `*` symbol.

```
myValue := 42
myPointer := &myValue

Print(*myPointer) // => 42

*myPointer = 23

Print(*myPointer) // => 23
```


## Functions

A function is a piece of code which takes any number of inputs, inclding 0,
performs actions, and optionally returns an output.

Rules:
- Declared in PascalCase.
- Exist at the top scope.
- Body must be declared within braces.
- Brace must be on the same line.
- Can optionally have a return type specifier before the name.

They can have their return type inferred.

```
Greeting(s name) {
	Print("Hello, %!", name)
}

// Later

Main() {
	Greeting("Sarah")
}
// => "Hello, Sarah!"
```

## Arrays

Arrays hold a sequence of things of the same type and are denoted by brackets.

Like Strings, they have access to a length value which is of type `u64`.

```
primes := [2, 3, 5, 7, 11, 13]

Type(primes) // => "u8[]"
Print(primes.length) // => 6
```
