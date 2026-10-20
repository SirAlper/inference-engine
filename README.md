# inference-engine

A neural network inference engine for running ONNX models on the **CPU**, written from scratch in C++20.

The goal is a small, readable engine that is accelerated with SIMD (AVX2/FMA) and multithreading, and whose correctness and speed are measured against [onnxruntime](https://onnxruntime.ai/). It sits in the same space as onnxruntime and llama.cpp, but is focused on learning and on showing how such an engine works from the ground up.

> **Status:** early development. The build system and unit tests are in place. `Storage` and `DType` are written and tested. `Tensor` is in progress: shape, row-major strides, storage allocation and typed element access (`data<T>()`) work; factories and views are next. Nothing runs end to end yet. See [Roadmap](#roadmap).

## Goals

- Load ONNX models directly (protobuf), with no runtime dependency on onnxruntime.
- Run real models end to end: an MNIST MLP first, then CNNs such as ResNet-18 and MobileNet.
- Optimize the hot paths: tiled GEMM, AVX2/FMA kernels, a custom thread pool, operator fusion and a memory planner.
- Verify every output against onnxruntime and publish reproducible benchmarks.

## Building

Requires a C++20 compiler, CMake ≥ 3.25, Ninja and Protobuf. It builds on macOS (Homebrew, Apple clang) and on Windows with MSYS2 UCRT64 (GCC 15).

```sh
cmake --preset macos-debug        # windows-debug on Windows
cmake --build --preset macos-debug
ctest --preset macos-debug
```

Use a `-release` preset for anything performance-related. Benchmarks are only meaningful in release builds.

Full setup instructions, including Homebrew and MSYS2 packages and VS Code configuration, are in [docs/building.md](docs/building.md).

### Build options

| Option | Default | Description |
| --- | --- | --- |
| `ENGINE_BUILD_TESTS` | `ON` | Build the GoogleTest unit tests |
| `ENGINE_BUILD_BENCHMARKS` | `ON` (`OFF` in presets for now) | Build the Google Benchmark suite |
| `ENGINE_ENABLE_AVX2` | `ON` on x86-64, `OFF` elsewhere | Compile `engine_core` with `-mavx2 -mfma` and define `ENGINE_HAS_AVX2` |

## Project layout

```
inference-engine/
├── CMakeLists.txt       root build file
├── CMakePresets.json    macos-* / windows-* presets (Ninja)
├── proto/               onnx.proto → onnx_proto library (planned)
├── src/                 engine_core static library
│   ├── tensor/          Storage, DType, Tensor
│   ├── loader/          ONNX loader, the only module that uses protobuf (planned)
│   ├── graph/           graph representation (planned)
│   ├── ops/             operator implementations (planned)
│   └── runtime/         executor, thread pool, memory planner (planned)
├── tests/               GoogleTest unit and correctness tests
├── benchmarks/          Google Benchmark suite (planned)
└── docs/                design and build documentation
```

## Roadmap

1. [ ] Tensor / Storage and naive reference operators
2. [ ] ONNX loader (protobuf)
3. [ ] Graph and executor: run an MNIST MLP end to end
4. [ ] CNN operators (Conv, Pool, BatchNorm, ...): ResNet-18 / MobileNet
5. [ ] Optimization: GEMM tiling, AVX2/FMA, thread pool, operator fusion, memory planner
6. [ ] Benchmarks against onnxruntime and a written report of the results

Details for each milestone are in [docs/roadmap.md](docs/roadmap.md).

## Documentation

- [docs/architecture.md](docs/architecture.md): modules, design decisions and the tensor memory model
- [docs/building.md](docs/building.md): toolchain setup, presets, IDE configuration and troubleshooting
- [docs/roadmap.md](docs/roadmap.md): milestones, their deliverables and how each is verified
