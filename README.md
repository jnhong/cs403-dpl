# The 403 Programming Language

A programming project for CS403.
```
println("Hello, world!"),
```

### Examples
Here are some examples of 403 code with ranging complexities. These examples can be found in `examples.403`.
Note: `==` is the assignment operator in 403, not the equality operator, which is `=`. They are switched in 403 on the grounds that this way appears more like conventional mathematics and discourages careless assignment.
```
! simple
square(x) := x * x :;
sqrt(x) := x ^ 0.5 :;
magnitude(v) := sqrt(square(v[0]) + square(v[1])) :;
v := allocate(2) :;
v[0] == 5, v[1] == 6,

println("The magnitude of vector <5,6> is ", magnitude(v), "."),

! classic
fib(n) :=
    if[ n < 2 ?
        n
      : else
        fib(n-1) + fib(n-2)
      ]
    :;

println("The 6th fibonacci number is ", fib(6)),

! what?
f := allocate(5) :;

{
i := 0:;
j := 0:;
 while[ : i < sizeof(f) :
        j == i mod 2,
        f[i] == lambda[x: lambda[y,z: return x * (y - z) + i * j, return 0]],
        i == i + 1
      ]
},

g(z) := if[ z < 1 ? f : else fib ] :;

println("1*(2-3) + 2*(2 mod 2) = ", g(0)[2](1)(2,3)),

! streams
stream_display(stream,n) :=
    helper(stream,i) :=
        if[ i < n ?
            print(scar[stream]),
            if[ i < (n-1) ?  print(",") ],
            print(" "),
            helper(scdr[stream],i+1)
          ]
        :;
    print("("),
    helper(stream,0),
    println("...)")
    :;

integers_from(n) := scons[n,integers_from(n+1)] :;

map(f,l) := scons[f(scar[l]),map(f,scdr[l])] :;

integers := integers_from(1) :;

squares := map(square,integers) :;

stream_display(squares,10),
```
More examples are given in the `.403` files.

### Interpreter
- The name of the interpreter is `403`, and it takes as an argument the filename of the input program to be processed. Example:
    - `403 program.403`
- The conventional filename extension for 403 code is `.403`.
- executable information (output of `file 403`):
  - ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 2.6.32, BuildID[sha1]=0d925514fe385cf5a79fbac807cffd91c8b2d43f, not stripped
- options
  - The `-p` option pretty-prints the input program.

### Terminology
Most terms have their usual meaning, but the following may be considered particularities:
- **body** - a body is a sequence of zero or more statements followed by an expression. The value of the body is considered to be the value of the last expression after evaluating the preceding statements in order.

### Rules of Thumb
- Brackets are used to denote special-forms or close relation to special-forms and are used by:
    - *lambda-expressions*
    - *if-expressions*
    - *while-expressions*
    - *stream functions*
- As will be seen below, colons are extensively used to delimit the components of many expressions.

## Language Basics

### Statements
- **Statements** are either a *declaration*, *definition*, or *expression* followed by a comma.
- Examples:
  - declaration
      - `x;`
          - The value of x is `UNINITIALIZED`.
  - definition
    - `x := 2 :;`
    - `x := lambda[a,b: a + b] :;`
    - `x(t) := 0.5 * t :;`
    - Notes:
      - Yes, the colon-semicolon is ugly, unfortunate effect of a flawed grammar. On the flip-side, one may view the content between the colons as a sort of "initialization" operator that sits between the identifier and semicolon.
        - `x;` → `x:=2:;`
      - Definition is not an operator in 403 and cannot be used in larger expressions. However, assignment is.
  - expression
    - `x,`
    - `x + 2,`
    - `x == x + 2,`
      - `==` is the assignment operator in 403, not the equality operator, which is `=`. They are switched in 403 on the grounds that this way appears more like conventional mathematics and discourages careless assignment.
    - `print(x),`

### Comments
- **Comments** are indicated using `!`, and extend to the next newline or `!`.
- `!!` indicates a "super-comment" that extends to the next newline.
- Examples:
  - `x == x + 1, ! this is a comment print(x),`
    - `print(x),` is commented out
  - `x == x + 1, ! this is a comment ! print(x),`
    - `print(x)` is not commented out
  - `x == x + 1, !! this is a super-comment ! print(x),`
    - `print(x)` is commented out

### Basic Data Types
- **integers**
  - `0, 1, 2, ...`
  - `-1, +1`
- **reals**
  - `0.0, 1.0, 3.14, ...`
  - `-1.0, +1.0`
- **boolean**
  - `true, false`
- **strings**
  - `"Hello, world!"`
  - escapable characters include `\n`, `\t`, `\b`, `\v`, `\f`, `\r`, `\a`, `"`, `\\`, with their usual meaning
  - attempting to escape a non-escapable character just returns the unmodified character
- other
  - `nil` is used to denote the empty list.

### Variables
- **Variables** are dynamically typed and must be declared before being used
- Valid names are any that begin with an alphabetical character followed by any contiguous sequence of alphanumerical or underscore characters

### Operators
- **unary operators**
  - prefix
    - positive, `+`
    - negative, `-`
    - not, `not` or `~`
  - postfix
    - array access, `[index]`, left
      - `a[0]`
      - Note: Array access also allows for an array element to be the left operand of assignment:
        - `a[5] == 0`
      - Note: This operator is applicable to anything that will evaluate to an array.
    - function application, `(arguments)`, left
      - `f(1)`
      - Note: This operator is applicable to anything that will evaluate to a closure.
    - Note: These can be mixed. For example:
      - `x[1](2)[3](4,5)`

- **binary operators**, with their associativity and precedence
  - mutative
    - assignment, `==`, right, 0
        - returns the right operand
  - relational
    - logical or, `or` or `+`, left, 1
        - short-circuiting
        - For typing reasons, the behavior of `+` slightly differs from `or` in that it returns `true` if either operand evaluates to `true` and returns `false` if both operands evaluate to `false`, whereas for `or`, non-`false` values are treated as `true`. This is for type checking operands of the addition operator.
    - logical and, `and` or `*`, left, 2
        - short-circuiting
        - For typing reasons, the behavior of `*` slightly differs from `and` in that it returns `false` if either operand evaluates to `false` and returns `true` if both operands evaluate to `true`, whereas for `and`, non-`false` values are treated as `true`. This is for type checking the operands of the multiplication operator.
    - equals, `=`, left, 3
    - not-equals, `~=`, left, 3
    - less-than, `<`, left, 4
    - greater-than, `>`, left, 4
    - less-than-or-equal-to, `<=`, left, 4
    - greater-than-or-equal-to, `>=`, left, 4
  - mathematical
    - plus, `+`, left, 5
        - applying `+` to string operands returns the concatenated string
    - minus, `-`, left, 5
    - modulo, `mod`, left, 6
        - `mod` computes the remainder of Euclidean division
    - div, `div`, left, 6
        - `div` computes the quotient of Euclidean division
    - multiplciation, `*`, left, 7
    - division, `/`, left, 7
    - remainder, `%`, left, 7
    - exponentiation, `^`, right, 6

### Functions
- **Functions** are first-class objects.
- Valid names are the same as for variables.
- **lambda-expressions**
  - The format of the lambda-expression is:
    - `lambda[ parameters : body ]`
    where the parameters are comma-separated
    - Examples:
      `lambda[x,y: x^2 + y^2]`
- There are two ways to define a function:
  - Using lambdas:
    - `f := lambda[x: x + x] :;`
  - Using the form `function-name(parameters) := body :;`:
    - `f(x) := x + x :;`
    which is syntactic-sugar for the first.
  - The return value of the function is the value of the body (see the section on terminology).
- Functions are called using the postfix function application operator `(arguments)`.
  - `f(1,2,3+4)`
- explicit returns
  - You can explicitly return from the body of a function, as you would in other languages, with a twist: return is a type of object, whose behavior is to short-circuit the execution of the body, as will be explained below.
  - Example:
    ```
    f(x) :=
        return 1 + 1,
        x := 1 :;
        x == x + 1,
        x
        :;
    ```
    - A return object is a return label paired with a value.
    - The value of a return object is the value of the expression to the right of the keyword `return`.
    - If a statement of a function body evaluates to a return object, the execution of the function body will be halted, the return label will be stripped from the return object, and the value of the return object will be returned as the value of the function.
    - As objects, return objects can be passed around as any other objects and thus can lead to very strange programs!

### Arrays
- Arrays are allocated using the built-in function `allocate(size)`, where `size` is the number of elements. The elements can be anything.
- Array elements are accessed using the postfix array access operator `[index]`.
- Examples:
  - `x == a[0] + 2,`
  - `a[1] == 2,`
- Note: The index can be a body of statements.

### Lists
- 403 provides the functions `cons()`, `car()`, and `cdr()`, which have their usual meaning.
- Also provided is `list(args)`, which returns the list composed of the evaluated arguments.
- Note: Lists are printable objects and have the expected format: `(item1, item2, ..., itemn)`

### Streams
- 403 similarly provides the stream versions of `cons()`,`car()`, and `cdr()`, called `scons[]`, `scar[]`, and `scdr[]`, respectively.
- For completeness, `delay[]` and `force[]` are also implemented.
- Note: These are provided as syntactic-sugar and are actually not at all necessary:
  - `delay[body]` is equivalent to `lambda[:body]`
  - `force[body]` is equivalent to `{body}()`
  - `scons[arg1,arg2]` is equivalent to `cons(arg1,lambda[:arg2])`
  - `scar[arg]` is equivalent to `car(arg)`
  - `scdr[arg]` is equivalent to `cdr(expr)()`
  these equivalences are quite literal, as the parser *literally* converts the former to the latter. (Try pretty printing it!)

### Conditionals
- Conditionals are specified using the **if-expression**, whose form is:
    - `if[ condition1 ? action1 : condition2 ? action2 : ... : else default-action ]`
    where there can be any positive integer number of condition-action pairs, and the default else-case is optional. Both the condition and action are bodies (see the section on terminology.)
    - Example:
        ```
        if[ x > 2 ?
            y == 2
          : x < 2 ?
            y == 5
          : else
            y == 0
          ]

        ```
- Being an expression, the if-expression can be used in larger expressions, for example:
    - `x == 2 + if[ x < 2 ? 5 : else 6 ]`
- The value of an if-expression is the value of the body of the action taken, and is `UNINITIALIZED` otherwise.

### Loops
- The only construct provided for iteration is the **while-expression**, whose form is:
    - `while[ pre-condition action : condition : post-condition action ]`
    where the action components are bodies without declarations/definitions and are both optional.
    - Note: Declarations are disallowed for obvious reasons.
- Being an expression, the while-expression can be used in larger expressions, for example:
    ```
    x;
    i := 0 :;
    n := 5 :;
    x == 5 + while[ : i < n : i == i + 1 ],
    ```
- The value of an while-expression is the value of the last expression evaluated thus far, and is `UNINITIALIZED` otherwise.


### Parenthesized Expressions and Blocks
- An expression can be parenthesized to raise its precedence and/or remove the ambiguity of a larger expression.
  - Example:
    - `x + (a + b)`
- A body of statements can be encapsulated in curly braces and treated as a unary whose value is the value of the final expression after evaluating the preceding statements in sequence. This is known as a **block**.
  - Example:
    - `x + { i := 2 :; i + i } - 5`
  - A block has its own scope. Thus, a block can be used to restrict the scope of variables:
    ```
    i := 2 :;
    {
        j := 5 :;
        i == i * j
    },
    ```

### Built-in Functions
- Normal
  - `print(args)` - prints the arguments, returns `UNINITIALIZED`
  - `println(args)` - prints the arguments, then a newline, returns `UNINITIALIZED`
  - `allocate(arg)` - returns an array object with integer `arg` number of elements, elements are `UNINITIALIZED` by default
  - `real(arg)` - coerces the numerical argument to type real
  - `integer(arg)` - coerces the numerical argument to type integer
  - `list(args)` - returns the list composed of the evaluated arguments
  - `cons(arg1,arg2)` - returns the cons of `arg1` and `arg2`
  - `car(arg)` - returns the car of the pair `arg`
  - `cdr(arg)` - returns the cdr of the pair `arg`
  - `display_env(arg)` - displays the given environment
    - To get the current environment, use the keyword `this`.
  - `sizeof(arg)` - returns the number of elements allocated to array `arg`
  - `is_uninitialized(arg)` - returns `true` if variable `arg` is `UNINITIALIZED`, `false` otherwise
  - `is_array(arg)` - returns `true` if `arg` is an array object, `false` otherwise
  - `is_closure(arg)` - returns `true` if `arg` is a closure object, `false` otherwise
  - `is_boolean(arg)` - returns `true` if `arg` is a boolean object, `false` otherwise
  - `is_pair(arg)` - returns `true` if `arg` is a pair, `false` otherwise
  - `is_list(arg)` - returns `true` if `arg` is a list object, `false` otherwise
  - `is_nil(arg)` - returns `true` if `arg` is `nil`, `false` otherwise
  - `is_string(arg)` - returns `true` if `arg` is a string, `false` otherwise
  - `is_integer(arg)` - returns `true` if `arg` is a integer number, `false` otherwise
  - `is_real(arg)` - returns `true` if `arg` is a real number, `false` otherwise
  - `is_return(arg)` - returns `true` if `arg` is a return object, `false` otherwise
- Special-form
  - `delay[body]` - equivalent to `lambda[:body]`
  - `force[body]` - equivalent to `{body}()`
  - `scons[arg1,arg2]` - equivalent to `cons(arg1,lambda[:arg2])`
  - `scar[arg]` - equivalent to `car(arg)`
  - `scdr[arg]` - equivalent to `cdr(expr)()`

### Other
- Absolute values can be specified using `|expression|`.
