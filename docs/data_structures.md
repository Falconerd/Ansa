# Data Structures

## Arrays

Arrays hold a sequence of things of the same type and are denoted by brackets.

Like Strings, they have access to a length value which is of type `u64`.

```
primes := [2, 3, 5, 7, 11, 13]

Type(primes) // => "u8[]"
Print(primes.length) // => 6
```

## Records

Records are a way of storing data which belongs together in the same place.

```
myPerson := { name: "Dawson Leery", age: 35 }

Print("% is % years old.", myPerson.name, myPerson.age)
// => "Dawson Leery is 35 years old."
```
