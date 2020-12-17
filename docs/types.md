# Types

The type system in Ansa allows you to build custom types from both primitives
and other custom types.

```c
type MyType = u32;

my_value : MyType = -1;
// => Compile error, unsigned integers cannot be negative
```

Let us look at a more realistic example using a record.

```c
type Person = (age u) {
	name s;
	age;
}

my_person := Person(4);

```

```c

type Sandwich = (slice_count u) {
	name s;
	weight f;
	flavour Flavour;
	slices MeatType[slice_count];
}

type Burger = (slice_count u) {
	name s;
	weight f;
	flavour Flavour;
	slices CheeseType[slice_count];
}

type Topping = CheeseType | MeatType

type Taco = (topping_count u) {
	name s;
	weight f;
	flavour Flavour;
	toppings Topping[topping_count];
}

```

```c
type Food = (slices u) Burger | Sandwich

type Sandwich (u number_of_meat_slices) {
	s name;
	f weight;
	Flavour flavour;
	MeatType[number_of_meat_slices] meat_slices;
}

type Burger (u number_of_cheese_slices) {
	s name;
	f weight;
	Flavour flavour;
	CheeseType[number_of_cheese_slices] cheese_slices;
}

type Food = Burger | Sandwich;

```
