
# 🧘 Zen Language Specification

Zen is a modern systems programming language focused on **simplicity**, **performance**, and **safety**. It blends familiar syntax with low-level control, without the baggage of legacy languages. Zen supports multiple paradigms including object-style methods, functional pipelines, and data-oriented layouts.

---

## 📚 Table of Contents

1. [Lexical Structure](#1-lexical-structure)  
2. [Core Types](#2-core-types)  
3. [Declarations](#3-declarations)  
4. [Control Flow](#4-control-flow)  
5. [Protocols & Methods](#5-protocols--methods)  
6. [Memory and Performance](#6-memory-and-performance)  
7. [Concurrency](#7-concurrency)  
8. [Foreign Function Interface (FFI)](#8-foreign-function-interface-ffi)  
9. [Standard Library](#9-standard-library-core-packages)

---

## 1. Lexical Structure

### 1.1 Tokens

- **Keywords**: `fn`, `record`, `type`, `protocol`, `impl`, `extend`, `pub`, `use`, `module`, `unsafe`, `if`, `else`, `while`, `for`, `in`, `yield`, `return`, `let`, `mut`, `match`, `try`, `spawn`, `await`, `scope`, `using`, `const`
- **Literals**:
  - Integer: `1`, `42`, `100_000`
  - Float: `3.14`, `0.5`, `1.2e-3`
  - String: `"hello, world"`
  - Character: `'c'`
  - Boolean: `true`, `false`
- **Operators**: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `!`, `&&`, `||`, `&`, `|`, `^`, `<<`, `>>`, `+=`, `-=`, `*=`, `/=`, `=`
- **Separators**: `(`, `)`, `{`, `}`, `[`, `]`, `,`, `.`
- **Semicolons**: Optional; newline or closing brace implies end of statement.

### 1.2 Comments

- Single-line: `//`

### 1.3 Visibility

- **Private**: Default for all declarations  
- **Public**: Use `pub` keyword

---

## 2. Core Types

| Category     | Type         | Description |
|--------------|--------------|-------------|
| Primitives   | `int`, `float`, `bool`, `byte`, `char`, `string` | `int` and `float` are 64-bit by default |
| Built-ins    | `T[]`, `Slice[T]`, `Map[K,V]`, `Set[T]`, `Option[T]` | Arrays, slices, maps, sets, and optional values |
| User Data    | `record`, `union` | Value types and tagged unions |
| Pointers     | `Ptr[T]`, `Box[T]`, `Rc[T]`, `Weak[T]` | Raw and managed pointers |

---

## 3. Declarations

### 3.1 Modules and Imports

```zen
// hello.zen
module app.hello
use core.io

fn main() -> int {
  println("hello, zen")
  return 0
}
```

### 3.2 Variables

```zen
let x = 10
let mut y = 20
y = 30
let name: string = "Zen"
```

### 3.3 Functions

```zen
fn add(a: int, b: int) -> int {
  return a + b
}
```

### 3.4 Records

```zen
record Vec2 { float x, y }
```

### 3.5 Type Aliases

```zen
type Bytes = byte[]
type Callback = fn() -> void
```

---

## 4. Control Flow

### 4.1 Conditionals

```zen
if (x > 10) {
  println("greater")
} else if (x < 10) {
  println("less")
} else {
  println("equal")
}
```

### 4.2 Loops & Comprehensions

```zen
fn sum_evens(xs: int[]) -> int {
  let mut acc = 0
  for x in xs if (x & 1) == 0 {
    acc += x * 2
  }
  return acc
}

fn sum_evens_comprehension(xs: int[]) -> int {
  return sum( for x in xs if (x & 1) == 0 yield x * 2 )
}
```

### 4.3 Pattern Matching

```zen
union Expr {
  lit(int),
  add(Box[Expr], Box[Expr]),
}

fn eval(e: Expr) -> int {
  match e {
    lit(n)    => n,
    add(a, b) => eval(*a) + eval(*b),
  }
}
```

---

## 5. Protocols & Methods

### 5.1 Protocols

```zen
protocol Order {
  fn cmp(self, other: Self) -> int
}
```

### 5.2 Impl Blocks

```zen
record Point { int x, y }

impl Order for Point {
  fn cmp(self, other: Point) -> int {
    return (self.x - other.x) != 0 ? self.x - other.x : self.y - other.y
  }
}
```

### 5.3 Extensions

```zen
extend Point {
  fn manhattan(self) -> int {
    return abs(self.x) + abs(self.y)
  }
}
```

---

## 6. Memory and Performance

### 6.1 Memory Model

- No garbage collector  
- Value semantics for `record` types  
- Ownership via `Box`, `Rc`, `Weak`  
- RAII with `using`

```zen
use core.fs

fn count_lines(path: string) -> Result[int] {
  using f = try File.open(path)
  let mut n = 0
  for line in f.lines() { n += 1 }
  return ok(n)
}
```

### 6.2 Error Handling

```zen
fn read_int(s: string) -> Result[int] { /* ... */ }

fn demo() {
  let n = try read_int("42")
  println(n)
}
```

### 6.3 Unsafe Blocks

```zen
unsafe fn poke(buf: byte[]) {
  let p = mem.ptr(buf)
  mem.store(p, 0, 65 as byte)
}
```

---

## 7. Concurrency

- `spawn`: Lightweight task  
- `await`: Suspend until completion  
- `scope`: Structured task management  
- `Channel[T]`: Type-safe communication

---

## 8. Foreign Function Interface (FFI)

```zen
foreign "c" {
  fn malloc(n: int) -> Ptr[void]
  fn free(p: Ptr[void]) -> void
}
```

---

## 9. Standard Library (Core Packages)

- `core.io`: I/O and streams  
- `core.fs`: File system  
- `core.net`: Networking  
- `core.concurrent`: Concurrency primitives  
- `core.ui`: UI toolkit  
- `core.db`: Database drivers  
- `core.mem`: Memory management

