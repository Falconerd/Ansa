# Data Structures

## Arrays

Arrays hold a sequence of things of the same type and are denoted by brackets.

Like Strings, they have access to a length value which is of type `u64`.

```
primes := [2, 3, 5, 7, 11, 13]

type(primes); // => "u8[]"
print(primes.length); // => 6
```

## Records

Records are a way of storing data which belongs together in the same place.

```
my_person := { name: "Dawson Leery", age: 35 };

print("% is % years old.", my_person.name, my_person.age);
// => "Dawson Leery is 35 years old."
```
