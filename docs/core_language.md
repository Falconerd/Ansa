# Core Language

There are three main features you need to be comfortable with to use Ansa:
**Values**, **References**, and **Functions**.

## Values

In Ansa, the most basic building blocks are values. This includes things such
as `23`, `35.5` and `"Hello there!"`.

Let's start by looking at the numeric types.

### Numbers

```
u some_number = 32;
i another_one = -1;
f and_another = 35.5;
```

The prefixes here are telling the compiler what type of number these are.

- `u` denotes an unsigned integer, which means the lowest number is 0.
- `i` denotes a signed integer, which means the number can be lower than 0.
- `f` denotes a floating point number, these can go below 0 and also contain
up to 7 decimal places.

You can let the compiler figure out (infer) the type by using the special `:=`
syntax.

```
what_am_i := 42;
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
how_big := 35000;
```

Since most of us are used to typing `int how_big = 35000`, we may expect that
this would be a signed integer type. However, that would be incorrect.

The type of the variable `how_big` would be `u16`.

By default the compiler will use unsigned integers. If it were signed integers,
then this value would have a type of `i32`, which is double the size.

If you need a variable to go below 0, make sure to define the type specifically
as as type starting with `i`.

### Matrices

Typically, a multi-dimensional array would be used to represent matrices.

However, ANSA has support for matrices up to 4x4 out of the box as their own
special types. This type comes with operators which make working with matrices
a breeze.

Matrices are denoted with the type `m` and similar to the number types, they
can be appended by one or two numbers. The first number is the row count, and
the second number is the column count. If no column count is declared, it will
be assumed the matrix has equal rows and columns.

Note: Matrices cannot have a row or column of 1. For that case, use a Vector.

- `m4` A matrix with 4 rows and 4 columns.
- `m42` A matrix with 4 rows and 2 columns.
- `m41` This is not valid, and a Vector should be used instead.

```
m4 transform = (1, 0, 0, 0,
				0, 1, 1, 0,
				1, 0, 1, 0,
				0, 0, 0, 1);
```

Matrices can have their type, rows, and columns inferred as well.

Note: Extra parentheses are required to help the compiler. This may or may
not be worth the inference.

```
my_matrix := (3, 2, 1, 4, 4, 6);
// => Error: Is this matrix 2x3 or 3x2?

my_matrix := ((3, 2, 1), (4, 4, 6));
type(my_matrix); // => m23

m23 my_matrix = (3, 2, 1, 4, 4, 6);
// This is actually 1 character shorter, but may be less clear at a glance!
```

In order to retrieve values from matrices, the letters `a` through `p` are
provided as properties on every matrix.

```
my_matrix := ((1, 2, 3), (4, 5, 6), (7, 8, 9));

print(my_matrix.c) // => 3
print(my_matrix.g) // => 7
```

Also keep in mind that since matrices cannot be 1-dimensional, if you need a
2x2, you must define it explicity or add the extra parentheses.

If you fail to do so, it will be considered a Vector.

```
my_matrix := (1, 2, 3, 4);
type(my_matrix); // => v4

my_matrix := ((1, 2), (3, 4));
type(my_matrix); // => m2
```

### Vectors

Vectors are essentially 1-dimensional matrices. However, they have their own
type, similar to matrices.

Vectors can contain between 2 and 4 (inclusive) values. They can always be
referenced using their `xyzw` properties. The order is set `x` then `y`
then `z` and finally `w`.

```
my_vec := (3, 4.3, -1, "hello there!");

print(my_vec.x); // => 3
print(my_vec.y); // => 4.3
print(my_vec.z); // => -1
print(my_vec.w); // => "hello there!"
```

Vectors have special operators which make them easier to use. The
multilplication operator `*` performs a cross product on two vectors, whereas
the dot `.` performs a dot product.

```
my_vec_a := (2, 3, 4);
my_vec_b := (5, -7, 3);

dot := my_vec_a . my_vec_b;
print(dot); // => 1

cross := my_vec_a * my_vec_b;
print(cross); // => (37, 14, -29)
```

Vectors can also be added and subtracted as you would expect.

```
my_vec_a := (2, 3, 4);
my_vec_b := (5, -7, 3);
my_vec_c := my_vec_a - my_vec_b;

print(my_vec_c); // => (-3, 10, 1)

print(my_vec_c + my_vec_a); // => (-1, 13, 5)
```

### Strings

The only non-numeric built in type is the string. Strings are denoted by their
type `s` and must be surrounded with quotation marks.

```
s greeting_text = "Hello there!";
```

They can also be inferred by the compiler using the special `:=` syntax.

```
greeting_text := "Hello there!";
```

Unlike C and some other languages, there is no `char` or `character` type.

```
some_char := 'a':
// This is NOT valid and will throw a compile error
char some_char = 'a';
// This is NOT valid and will throw a compile error
```

However, if a string with one character that is defined in the ASCII standard
is defined, it will be converted into a `u8` behind the scenes.

This is because the string type contains an internal length counter, which is
a `u64`.
```
some_char := "a";

type(some_char); // => "u8"
print(some_char); // => a
print(some_char); // => 61
size(some_char); // => 8

two_chars := "ab";

print(two_chars); // => ab
print(two_chars.length); // => 2
type(two_chars.length); // => "u64"
size(two_chars); // => 80
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
my_value := 42;

---------------------------------
|   A   |   B   |   C   |   D   |
---------------------------------
|       |  42   |       |       |
---------------------------------

print(&my_value); // => 132
```

References can be declared like other values in order to pass them around. For
this, the special `*` syntax is used after the type in conjunction with the
`&` syntax in front of the value name.

```
i32 my_value = 42;
i32* my_ref = &myValue;

print(&my_value); // => 150 (for example)
print(my_ref); // => 150

```

Using the reference, the value can be obtained by once again using the `*`
syntax, but this time in front of the value name.

```
i32 my_value = 42;
i32* my_ref = &myValue;

print(*my_ref); // => 42
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
greeting(s name) {
	print("Hello, %!", name);
}

// Later

main() {
	greeting("Sarah");
}
// => "Hello, Sarah!"
```
