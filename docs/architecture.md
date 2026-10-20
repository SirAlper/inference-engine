# Architecture

This document describes how the engine is structured and why. Items marked *(planned)* are designed but not implemented yet.

## Overview

```
 .onnx file ──► loader ──► graph ──► runtime (executor) ──► output tensors
                  │                        │
              protobuf                    ops ──► tensor
```

Everything except the command-line tool lives in a single static library, `engine_core`. Tests, benchmarks and the CLI link against it.

| Module | Responsibility | Status |
| --- | --- | --- |
| `tensor/` | `Storage` (owned, aligned memory) and `Tensor` (typed, shaped view over a storage) | in progress |
| `loader/` | Parses `.onnx` files into the engine's graph representation | planned |
| `graph/` | Nodes, edges, initializers (weights), topological order | planned |
| `ops/` | Operator implementations; all math lives here | planned |
| `runtime/` | Executor; later the thread pool and memory planner | planned |

## Build targets

| Target | Kind | Notes |
| --- | --- | --- |
| `engine_core` | static library | Exposes `src/` as a PUBLIC include directory, so headers are included as `"tensor/storage.hpp"` |
| `engine_warnings` | INTERFACE library | Carries `-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor`; linked PRIVATE so warnings apply only to our own sources |
| `onnx_proto` | static library *(planned)* | Generated from `proto/onnx.proto`; linked PRIVATE to `engine_core` |
| `engine_tests` | executable | GoogleTest unit tests from `tests/`; built when `ENGINE_BUILD_TESTS` is on |
| `engine_cli` | executable *(planned)* | Command-line tool for running a model |

### Protobuf isolation

Protobuf is an implementation detail of the loader. `onnx_proto` is linked to `engine_core` as **PRIVATE**, and no public header includes a generated `.pb.h` file. As a result:

- consumers of `engine_core` never see protobuf types or need its include paths;
- the rest of the engine works only with its own graph and tensor types;
- the protobuf dependency could later be replaced without touching other modules.

Protobuf is found with `find_package(Protobuf CONFIG REQUIRED)`. CONFIG mode is required because recent protobuf versions depend on abseil, which only the package's own CMake config resolves correctly.

### SIMD flags

When `ENGINE_ENABLE_AVX2` is on, `engine_core` is compiled with `-mavx2 -mfma` and `ENGINE_HAS_AVX2=1`. These are PRIVATE to `engine_core`. Intrinsics must therefore only appear in `.cpp` files: if an inline function in a header were compiled with different flags in different translation units, that would be an ODR violation.

## Tensor memory model

### Storage

`Storage` (`src/tensor/storage.hpp`) owns a single raw allocation.

- Members: `std::byte* data_` and `std::size_t nbytes_`.
- Memory is aligned to `Storage::kAlignment` (64 bytes) using aligned `::operator new` / `::operator delete`. 64 bytes is one cache line and covers the alignment needs of AVX2 (32 bytes) and AVX-512 (64 bytes) loads.
- A zero-byte `Storage` allocates nothing; its `data()` is `nullptr`.
- `data()` has a const and a non-const overload, so a `const Storage&` only gives read access to the bytes.
- Copying and moving are deleted: a `Storage` is a unique owner of its memory and never changes address. Sharing happens one level up, through `std::shared_ptr<Storage>`.

### DType

`DType` (`src/tensor/dtype.hpp`) is a one-byte `enum class` naming the element type of a tensor at run time.

- Values: `Float32` and `Int64`.
- `dtype_size(DType)` returns the element size in bytes, and `dtype_name(DType)` returns `"float32"` / `"int64"`. Both are `constexpr`.
- `dtype_of<T>` / `dtype_of_v<T>` map a C++ type to its `DType` at compile time: `float` → `Float32`, `std::int64_t` → `Int64`. The primary template has no definition, so an unsupported type such as `double` fails to compile instead of being accepted silently.

### Tensor *(in progress)*

`Tensor` (`src/tensor/tensor.hpp`) is a non-templated class whose element type is chosen at run time.

- Describes data with a **shape**, **strides** and a **dtype**. The constructor rejects negative dimensions with `std::invalid_argument`, computes `numel`, sets row-major strides (in elements) and allocates a `Storage` of `numel * dtype_size(dtype)` bytes. The memory is not initialized.
- Holds its data as `std::shared_ptr<Storage>` plus an **offset**. The offset is counted in elements, like the strides, so the address of element `(i, j)` is `base + offset + i*stride[0] + j*stride[1]`.

#### Typed access: `data<T>()`

The dtype is only known at run time (it comes from the model file), but a C++ pointer type has to be known at compile time. `data<T>()` connects the two:

```cpp
engine::Tensor t({2, 3}, engine::DType::Float32);
float* p = t.data<float>();      // OK
t.data<std::int64_t>();          // throws std::invalid_argument
```

1. It compares `dtype_of_v<T>` with the tensor's dtype and throws `std::invalid_argument` if they differ.
2. It reinterprets the storage's `std::byte*` as `T*`.
3. It adds the element offset. The offset is added **after** the cast, so pointer arithmetic moves in units of `T`.

There are two overloads. On a `const Tensor`, `data<T>() const` returns `const T*`, so read-only code cannot write through the pointer. The const overload goes through a `const std::byte*`, because `shared_ptr::operator->` does not carry the tensor's constness over to the `Storage` it points to.

Since the storage base is 64-byte aligned and the offset is a whole number of elements, the pointer returned for a tensor with offset 0 is 64-byte aligned.

#### Views *(planned)*

Because several tensors can point at the same `Storage` with different offsets, shapes and strides, operations such as reshape, transpose and slicing can produce **views without copying** any data. The `Storage` is freed when the last tensor referring to it goes away.

`Tensor` holds no math. Arithmetic and every other operator live in `ops/`, which keeps the data container small and lets operators evolve (naive first, SIMD later) without changing the tensor API.

## Verification

- **Correctness:** outputs are compared against onnxruntime's outputs for the same model and inputs, in GoogleTest tests.
- **Performance:** measured with Google Benchmark, always in release builds.
