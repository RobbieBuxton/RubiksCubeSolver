# Contributing Guide

## C Code Style
We are using a slight derivative on the K&R style (OTBS variant) for C.

```c
#define NUMBER_FIVE 5
#define InRange(l, h, v) (l <= v && v <= h)

static const int NUMBER_TEN = 10;

typedef int number;
typedef char* string;
typedef unsigned int ind_t;

typedef struct {
    double x;
    double y;
} Point;

typedef union {
    int i;
    float f;
} Real;

/**
 * Function that adds a and b
 *
 * @param  a left hand value
 * @param  b right hand value
 * @return   a + b
 */
int add_func(int a, int b) {
    // Adds a and b
    return a + b;
}

/**
 * Prints something different depending on the given number.
 *
 * @param  i Decider number for printing
 * @remark   Prints "This number is (large|small)" to the stdout
 */
void conditional_printer(int i) {
    if (i > 100) {
        printf("This number is large\n");
    } else {
        printf("This number is small\n");
    }
}

/**
 * Does nothing important.
 */
void looper(void) {
    for (int i = 0; i < 5; ++i) {
        // Something goes here
    }

    int i = 4;
    do {
        --i;
    } while (i >= 0);

    while (1) {
        break;
    }
}

/**
 * Sums a 3-long array of ints.
 *
 * @param  nums A 3+ long array of ints
 * @return      nums[0] + nums[1] + nums[2]
 */
int three_sum(int nums[static 3]) {
    return nums[0] + nums[1] + nums[2];
}
```

### Spacing
* Indents use 4 spaces.
* Array initialisers have spaces between their braces and their contents. E.g. `int ints[] = { 1, 2, 3 };`.
* Parentheses do not have spaces between their internal values. E.g. `(a)` or `void func(int b)`.
* Brackets do not have spaces between their internal values. E.g. `arr[5]`.
* Statement / function body braces are laid out as the OTBS style:
    * Opening braces lie on the same line as the parameter list and type / condition, and have a space after the closing parenthesis of the parameter list / condition.
    * Closing braces lie on the line after the last statement in the body.
    * `else`, `else if` and `while` (in do-while loops) are placed on the same line as the closing brace.
* There is one space after a `,`.
* There is one space after a `;` in a for loop declaration, unless putting two `;`s next to each other.
* Avoid having more than one blank line between things.
* If possible, purge blank lines of any whitespace:

```
(. represents a space)
OK:

....code
....
....more code

BETTER:

....code

....more code
```

### Lines
* One statement per line, with the exception of some multiple declarations (see below).
* There is no hard column limit, just be sensible about your line lengths.
    * 120 characters per line is a soft guide.
* Separating statements in parentheses, brackets or braces is done line-by-line:

```c
int grid[2][2] = {
    { 1, 2 },
    { 3, 4 }
};

char* allocd = (char*) calloc(
    100,
    sizeof(char)
);

// etc.
```

### Naming
* Function names, variables and parameters are written in `snake_case`.
* Constant names and constant macros are written in `SCREAMING_SNAKE_CASE`.
* Macros for code segments (and parameterised macros) are written in `PascalCase`.
* Typedefs for non-struct / non-union types are all in lowercase, and can optionally have the `_t` suffix.
* Typedefs for struct / union / function pointer types (and struct / union types in general) are written in `PascalCase`.
* Structs and unions are always `typedef`-ed (to spare one from needing the `struct` and `union` keyword everywhere).
* Pointers are associated with the variable, so `int *iptr` and not `int* ptr` nor `int * iptr`.
    * However, do not mix pointers and non-pointers in the same declaration:

```
// OK
int a, b;
int *c, *d;

// NOT OK
int a, b, *c, *d;
```

### Commenting
* No inline comments. Only comments preceding lines.

### Documentation Comments
* We are using Doxygen to generate documentation files. Hence we will use a format allowed by doxygen; Javadoc-style comments:

```c
/**
 * Multiplies two ints together and returns their result.
 *
 * @param  a Multiplicand
 * @param  b Multiplier
 * @return   a * b
 */
int func(int a, int b) {
    return a * b;
}
```

* The descriptions for @-tags must be vertically aligned.
* The parameters for `@param` tags must be vertically aligned.
* There must be room between the largest @-tag name and the parameters for `@param` tags.
* For more information on the useable tags, see [the official Doxygen documentation](https://www.doxygen.nl/manual/commands.html).

### Function Parameters
* Functions that have no parameters must have a `void` in its parameter list: `void f(void) { }`.
* The use of `[size]` or `[static size]` to indicate minimum size of array to pass into a function is allowed, and is advised.
* Function pointers should be `typedef`-ed to minimise the mess they create:

```c
typedef int (*IntMapper)(int);

void mapInt(int *ints, size_t amt, IntMapper mapper) {
    for (size_t i = 0; i < amt; ++i) {
        ints[i] = mapper(ints[i]);
    }
}
```

### Miscellaneous
* Multiple declarations are allowed, provided that they are sensible. E.g. `int i = 0, j = 0;` when looping over a two-dimensional array.
* For-Loops are allowed to omit any of their three statements, provided that suitable substitutes are given:

```c
int i = 0;
for (; i < 10; ++i) { }

// OR

for (int i = 0;; ++i) {
    if (i == 100) {
        break;
    }
}

// OR

for (int i = 1; i < 100;) {
    if (i < 100) {
        i *= 2;
    } else {
        i += 10;
    }
}
```

* One line statements go in braces:

```c
// Okay
if (cond1) {
    stmt1;
} else if (cond2) {
    stmt2;
} else {
    stmt3;
}

for (;;) {
    stmt;
}

// Not Allowed
if (cond) stmt;

if (cond)
    stmt;

if (cond1) {
    stmt;
} else stmt2;
```
* Empty functions use same-line spaced braces. E.g. `void f(void) { }`.
* Case fallthrough in switches are allowed.
* Use `do { ... } while (1)` loops instead of `while (1) { ... }` loops as they convey your intention a little better.
* Due to the nature of `a[b]` expanding to `*(a + b)`, always make sure `a` is a pointer and `b` is a suitable indexer (e.g. `int` or `size_t`).
* If possible, avoid `*(a + b)` entirely.
* When writing array initialisers, the size can be omitted:

```c
// Both are OK

int ints[5] = { 1, 2, 4, 6, 7 };
int nums[] = { 1, 2, 4, 6, 7 };
```

* `goto` is allowed for breaking out of nested loops and for error handling.
