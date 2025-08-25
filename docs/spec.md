# 🧘 Quastra Language Specification v2 (Final)

Quastra is a modern **systems programming language** designed for **performance, safety, and clarity**. It is fully **type-safe**, has **minimal syntax**, **built-in concurrency**, and is **extensible for advanced systems programming**, suitable for everything from **OS kernels to game engines, high-performance backends, and desktop apps**.

---

## 🎯 Design Goals

1. **Type-safety and predictability**

   * Prevent undefined behavior and memory corruption.
   * Explicit ownership semantics.
   * Optional heap allocation, stack-first by default.

2. **Minimal syntax, readable**

   * Lean language constructs.
   * Inline methods in records, optional `impl` blocks.
   * No unnecessary boilerplate.

3. **High-performance**

   * Deterministic memory layout.
   * No garbage collector.
   * Low-level primitives via optional modules (`unsafe`, `core.mem`).

4. **Built-in concurrency**

   * Lightweight tasks (`spawn`) and structured concurrency (`scope`).
   * Channels for safe communication.

5. **Extensible**

   * Advanced features (FFI, unsafe memory ops, advanced synchronization) are optional modules.

6. **General-purpose**

   * Suitable for OS kernels, game engines, high-performance servers, web apps, and more.

---

## 1️⃣ Lexical Structure

### 1.1 Tokens

* **Keywords**:
  `fn`, `record`, `union`, `protocol`, `impl`, `extend`, `pub`, `use`, `module`, `unsafe`, `if`, `else`, `while`, `for`, `in`, `yield`, `return`, `let`, `mut`, `match`, `try`, `spawn`, `await`, `scope`, `using`, `const`, `Option`, `some`, `none`

* **Operators**:
  Arithmetic: `+`, `-`, `*`, `/`, `%`
  Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
  Logical: `!`, `&&`, `||`
  Bitwise: `&`, `|`, `^`, `<<`, `>>`
  Assignment: `=`, `+=`, `-=`, `*=`, `/=`

* **Separators**: `(`, `)`, `{`, `}`, `[`, `]`, `,`, `.`

* **Literals**:

  * Integer: `42`, `100_000`
  * Float: `3.14`, `1.2e-3`
  * Boolean: `true`, `false`
  * Character: `'c'`
  * String: `"hello"`

* **Comments**:

  * Single-line: `//`
  * Multi-line: `/* ... */`

* **Semicolons** are optional; newlines or closing braces imply end of statement.

---

## 2️⃣ Types

| Category   | Types                                     | Notes                                        |
| ---------- | ----------------------------------------- | -------------------------------------------- |
| Primitives | `int`, `float`, `bool`, `char`, `string`  | 64-bit defaults                              |
| Compound   | `record`, `union`, `array[T]`, `slice[T]` | Value types, tagged unions, resizable arrays |
| Generic    | `Option[T]`                               | Type-safe optional values, replaces `null`   |
| Pointers   | `Ptr[T]`, `Box[T]`, `Rc[T]`, `Weak[T]`    | Raw & managed heap allocation, optional      |
| Function   | `fn(T1,T2...) -> R`                       | Typed first-class functions                  |
| Protocols  | `protocol`                                | Type-safe polymorphism (interfaces)          |

**Notes**:

* Stack-first allocation by default for speed.
* Heap allocation is explicit via `Box`.
* `Option[T]` avoids null references.

---

## 3️⃣ Declarations

### 3.1 Modules and Imports

```quastra
module app.main
use core.io

fn main() -> int {
    println("Hello Quastra!")
    return 0
}
```

### 3.2 Variables

```quastra
let x = 10        // immutable
let mut y = 20    // mutable
let name: string = "Quastra"
```

### 3.3 Functions

```quastra
fn add(a: int, b: int) -> int {
    return a + b
}

// Inline methods in records
record Vec2 { x: float, y: float }
fn (v: Vec2) magnitude() -> float {
    return sqrt(v.x*v.x + v.y*v.y)
}
```

### 3.4 Records / Unions

```quastra
record Point { x: int, y: int }

union Expr {
    Lit(int),
    Add(Box[Expr], Box[Expr])
}
```

---

## 4️⃣ Control Flow

```quastra
if x > 0 {
    println("positive")
} else if x < 0 {
    println("negative")
} else {
    println("zero")
}

for i in 0..10 {
    println(i)
}

while x > 0 {
    x -= 1
}

match expr {
    Lit(n) => n,
    Add(a,b) => eval(*a) + eval(*b)
}
```

---

## 5️⃣ Protocols & Polymorphism

```quastra
protocol Comparable {
    fn cmp(self, other: Self) -> int
}

record Point { x: int, y: int }
fn (p: Point) cmp(self, other: Point) -> int {
    return if self.x != other.x { self.x - other.x } else { self.y - other.y }
}
```

* Optional `impl` blocks for grouping methods if desired.

---

## 6️⃣ Memory & Resource Management

* **Stack-first allocation** by default.
* **Heap allocation** via `Box[T]`.
* **RAII / using** ensures deterministic resource cleanup:

```quastra
using f = File.open("data.txt")
for line in f.lines() {
    println(line)
} // file automatically closed
```

* Optional `unsafe` module for pointer arithmetic / advanced memory ops.

---

## 7️⃣ Concurrency

* Lightweight tasks: `spawn { ... }`
* Structured concurrency: `scope { ... }`
* Channels: `Channel[T]` for safe communication

```quastra
let ch: Channel[int] = Channel()

spawn {
    for i in 1..5 { ch.send(i) }
    ch.close()
}

spawn {
    for x in ch { println(x) }
}

await scope
```

---

## 8️⃣ Error Handling

* `Result[T]` type with `ok` / `err` variants
* `try` / `catch` optional; use `try` for propagation:

```quastra
fn read_int(s: string) -> Result[int] { ... }

fn demo() {
    let n = try read_int("42")
    println(n)
}
```

---

## 9️⃣ Generics

```quastra
fn swap[T](a: T, b: T) -> (T,T) { return (b,a) }
```

* Type-safe, fully generic functions and records.
* Constraints via protocols: `fn f[T: Comparable](a:T,b:T) -> T { ... }`

---

## 10️⃣ Standard Library (Core)

| Module            | Purpose                           |
| ----------------- | --------------------------------- |
| `core.io`         | File & console I/O                |
| `core.fs`         | File system operations            |
| `core.net`        | Networking                        |
| `core.concurrent` | Tasks, Channels, Atomic counters  |
| `core.mem`        | Memory utils, allocators          |
| `core.math`       | Linear algebra, vectors, matrices |
| `core.opengl`     | 3D rendering                      |
| `core.db`         | Database drivers                  |
| `core.ui`         | Optional UI toolkit               |

---

## 11️⃣ Optional Advanced Modules

* `core.unsafe` → pointer arithmetic, manual memory ops
* `core.futures` → async/await with high-performance tasks
* `core.crypto` → cryptography
* `core.sys` → low-level OS interactions

---

## 12️⃣ Concurrency & Parallelism Philosophy

* **Built-in lightweight tasks** instead of OS threads.
* **Channels** instead of locks by default.
* **Structured concurrency** ensures safe resource lifetimes.

> These features allow Quastra to power **OS kernels, game engines, and high-performance backend servers** safely.

---

## 13️⃣ Summary of Language Philosophy

* **Safety**: No nulls, enforced types, predictable memory layout.
* **Performance**: Stack-first, optional heap, deterministic.
* **Simplicity**: Minimal keywords, readable, inline methods.
* **Extensible**: Advanced systems primitives optional.
* **Concurrency-first**: Tasks, channels, structured scopes.
* **General-purpose**: From OS kernels to games to web servers.

