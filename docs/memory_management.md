# Memory Management

```c
// A simple linked-list
type node_data { id u32, data any };
type node { next node*, data node_data* };

type LinkedList a = Empty | Node a (List a)

type linked_list a = empty | node a (linked_list a)

type LinkedList a = Empty | Node a (LinkedList a);

// OOP
// Either: every object of this type has a version of this function
// Or: every object of this type has a pointer to this function
// Or: the compiler has a pointer to this function
my_list := [1, 2, 3, 4].reverse().splice(1);

// Procedural
my_list := list_splice(1, list_reverse([1, 2, 3, 4]));

// Procedural with reassignment
my_list := [1, 2, 3, 4];
my_list = list_reverse(my_list);
my_list = list_splice(1, my_list);

// Pipes
my_list := [1, 2, 3, 4] |> list_reverse |> list_splice <| 1;

// UFCS v1
my_list := [1, 2, 3, 4].reverse().splice(1);
// UFCS v2
my_list := [1, 2, 3, 4].reverse.splice(1);


// What if types their own methods??

type Robot (id u) {
	id;
	say_hello: () => print("Hello there! My name is %.", id);
}

my_robot := Robot(1234);
my_robot.say_hello(); // => "Hello there! My name is 1234."

// Now we have just reinvented classes...


// Types can be nothing but identifiers
type True
type False

// Which can later be used in unions
type Bool = True | False


type Robot (id u) {
	id;
}

// What about partial types?
part WithId {
	id u;
}

// This function accepts anything with an id field
// Also added destructuring for kicks
say_hello :: ({ id } WithId) => print("Hello there! My name is %.", id);

// Same as above, with type in the declaration
say_hello_2 :: ({ id } { id u }) => print("Hello there! My name is %.", id);

my_robot := Robot(1234);

say_hello(my_robot); // => "Hello there! My name is 1234."
say_hello_2(my_robot); // => "Hello there! My name is 1234."

type Animal (id u, legs, eyes) {
	id, legs, eyes;
}

shale := Animal(20, 3, 2);

say_hello_2(shale); // => "Hello there! My name is 1234."

// Let us introduce pipes for no reason
// After this line, the Robot is destroyed
say_hello <| Robot(1234); // => "Hello there! My name is 1234."

// Likewise here
Robot(1234) |> say_hello; // => "Hello there! My name is 1234."

my_robot |> say_hello; // => "Hello there! My name is 1234."







part WithId {
	id u;
}


// This type takes another type 'a' as a parameter
type Node a {
	edges *Node[];
	data a;
}

// Linked lists use nodes with one or two edges per node
type LinkedList {
    head *Node;
    length u32;
    item_size u64;
}

// Graphs also use nodes with any number of edges
type Graph {
	nodes *Node[];
	...
}









```
