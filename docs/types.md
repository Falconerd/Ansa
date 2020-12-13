# Types

The type system in Ansa allows you to build custom types from both primitives
and other custom types.

```
type myType = u32

myType myValue = -1
// => Compile error, as unsigned integers cannot be negative
```

Let us look at a more realistic example using a record.

```
type person = { s[80] name, u8 age }
```

You will notice that we must define the length of the string here, that is because

