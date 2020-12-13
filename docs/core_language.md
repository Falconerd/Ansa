# Core Language

There are three main features you need to be comfortable with to use ANSA:
**Values**, **References**, and **Functions**.

## Values

In ANSA, the most basic building blocks are values. This includes things such
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
| `u64`  | 0   | 2^63 - 1 |
| `i8`   | -128 | 127 |
| `i16`  | -32 768 | 32 767 |
| `i32`  | -2 147 483 648 | 2 147 483 647 |
| `i64`  | -2^63 | 2^63 - 1 |


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
Print(someChar) // => a
Print(someChar) // => 61
Size(someChar) // => 8

twoChars := "ab"

Print(twoChars) // => ab
Print(twoChars.length) // => 2
Type(twoChars.length) // => "u64"
Size(twoChars) // => 80
```

## References

References are special values but rather than holding a number or string, they
hold the memory address of a different value.

In order to understand references, you need to know a little bit about how
values are stored in memory.

Here we have a representation of 4 "blocks" of memory. Let us assume that each
block is 32-bit, enough to hold a standard integer or float.

We will also assume that the memory address of A starts at 100, to simplify
things.

```
        v-132           v-196
---------------------------------
|   A   |   B   |   C   |   D   |
---------------------------------
^-100           ^-164
```

As you can see, each block increases the memory address number by 32, as they
are 32-bit blocks.

This is in essence how all memory in a computer works. When we declare a value,
the value is stored into one of these blocks, and then we get a reference to
the "address" of the block.

When we call on the value normally, it gives us the data inside, but if we use
the special `&` syntax in front of the name, then we can get the address
instead.

```
myValue := 42

---------------------------------
|   A   |   B   |   C   |   D   |
---------------------------------
|       |  42   |       |       |
---------------------------------

Print(&myValue) // => 132
```

References can be declared like other values in order to pass them around. For
this, the special `*` syntax is used after the type in conjunction with the
`&` syntax in front of the value name.

```
i32 myValue = 42
i32* myRef = &myValue

Print(&myValue) // => 150 (for example)
Print(myRef) // => 150

```

Using the reference, the value can be obtained by once again using the `*`
syntax, but this time in front of the value name.

```
i32 myValue = 42
i32* myRef = &myValue

Print(*myRef) // => 42
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
